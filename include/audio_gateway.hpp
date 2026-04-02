#pragma once
/**
 * @file Gateway.hpp
 * @brief High-Performance Audio I/O Engine for VITA.
 * * ARCHITECTURAL DECISIONS:
 * 1. PRODUCER-CONSUMER PATTERN: We use SPSC Lock-Free Ring Buffers to decouple
 * Hardware IRQs (Interrupt Requests) from Network I/O. This prevents a slow
 * network from "stalling" the audio hardware, which would cause audible pops/clicks.
 * * 2. NO IN-ENGINE VAD (STATE MACHINE): Voice Activity Detection and Level Analysis
 * are intentionally OMITTED here. To maintain "Super Developer" C++23
 * determinism, this gateway acts as a "dumb pipe." All VAD logic is handled
 * by the STT Node to keep this container's CPU footprint perfectly flat.
 * * 3. ZMQ HARDENING: We use High-Water Marks (HWM) and Linger settings to manage
 * backpressure. If the STT node crashes, the Gateway buffers up to 60s
 * of audio in the ZMQ layer before dropping frames, preventing OOM.
 * * 4. HARDWARE AFFINITY: Each thread is pinned to a specific CPU core (CPU Pinning).
 * This eliminates "Context Switching" overhead and ensures the L1 cache
 * stays "warm" with audio data, critical for 100ms real-time constraints.
 */
#include <alsa/asoundlib.h>
#include <array>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
#include <expected>
#include <memory>
#include <optional>
#include <thread>
#include <vector>
#include <zmq.h> // Using C-API directly to guarantee NO EXCEPTIONS

#include "config.hpp"

/**
 * @brief Error Taxonomy for Functional Flow
 * Defines specific hardware and logic failure states for std::expected pipelines.
 * Using a scoped enum (enum class) prevents namespace pollution and forces explicit casting.
 */
enum class SystemError : uint8_t
{
    AlsaOpenFailed,       // Device busy or not found
    AlsaConfigFailed,     // Hardware does not support requested sample rate/format
    AlsaOverrun,          // CPU failed to pull data fast enough (XRUN)
    ZmqSocketError,       // Network transport layer failure
    ThreadAffinityFailed, // Unable to pin thread to specific CPU core
    ThreadPriorityFailed, // Unable to set SCHED_FIFO (usually permission/cap_add issue)
    QueueFull,            // SPSC queue at capacity (Network or IO bottleneck)
    QueueEmpty            // SPSC queue contains no data (Producer lag)
};

/**
 * @brief Static Audio Chunk
 * We use std::array instead of std::vector to ensure the memory is
 * contiguous and fixed-size, eliminating runtime heap churn.
 * This is a huge win for the L1/L2 cache on the Radxa's ARM processor
 * because the data is stored "in-line" with the actual_frames metadata.
 */
struct AudioChunk
{
    std::array<int16_t, Schema::k_frames_per_buffer> data{};
    size_t actual_frames = 0;
};

/**
 * @brief RAII Custom Deleter for ALSA
 * Ensures that the PCM device is closed immediately when the UniquePcm goes out of scope.
 * This prevents the "Device or resource busy" error on container restarts.
 */
struct AlsaDeleter
{
    void operator()(snd_pcm_t* handle) const noexcept
    {
        if (handle != nullptr) {
            snd_pcm_close(handle);
        }
    }
};

/**
 * @brief RAII PCM Handle
 * A type-safe, non-copyable smart pointer for managing ALSA hardware lifecycle.
 */
using UniquePcm = std::unique_ptr<snd_pcm_t, AlsaDeleter>;

/**
 * @brief AudioPool (Lock-Free Circular Buffer)
 * SPSC (Single Producer Single Consumer) queue tailored for high-frequency audio data.
 * The capacity is baked into the type via Schema::POOL_CAPACITY to allow
 * the compiler to optimize the internal modulo arithmetic for ring-buffer indices.
 */
using AudioPool =
    boost::lockfree::spsc_queue<AudioChunk*, boost::lockfree::capacity<Schema::k_pool_capacity>>;

/**
 * @brief The Functional Audio Gateway
 * Orchestrates four real-time threads: Mic-In, ZMQ-Out, ZMQ-In, Speaker-Out.
 * Designed as a "Zero-Allocation" engine where all memory is claimed at boot
 * and recycled through free/data queue pairs to avoid the OS allocator during runtime.
 */
class AudioGateway
{
public:
    /**
     * @brief Construct the Gateway with shared logging and runtime config.
     * Marks as noexcept as we pre-allocate memory in a controlled loop without throwing.
     */
    explicit AudioGateway(const GatewayConfig& config, class ModuleLogger& log) noexcept;

    /**
     * @brief Disable copy/assignment.
     * Hardware gateways manage exclusive resources (ALSA devices); copying would lead to
     * multiple threads fighting for the same hardware file descriptor.
     * This is a "Hardware Singleton" pattern.
     */
    AudioGateway(const AudioGateway&) = delete;
    auto operator=(const AudioGateway&) -> AudioGateway& = delete;

    /**
     * @brief Cleanly stops all threads and releases hardware handles.
     */
    ~AudioGateway();

    /**
     * @brief Spawns threads and enters the main execution state.
     * This method triggers the hardware initialization and thread hardening (RT Priority).
     */
    void start();

    /**
     * @brief Signals all threads to stop and joins them gracefully.
     * Sets the atomic 'running' flag to false, allowing loops to exit their current cycle.
     */
    void stop() noexcept
    {
        running_ = false;
    }

private:
    const GatewayConfig config_; // Const to prevent accidental runtime modification
    ModuleLogger& log_;          // Reference to external logger (Gateway does not own lifecycle)

    /** @brief Main loop control flag; atoms ensure visibility across all CPU cores. */
    std::atomic<bool> running_{true};

    /** @brief Container for worker threads to facilitate clean joining during shutdown. */
    std::vector<std::thread> workers_;

    /** * @brief Queues for decoupling hardware and network layers.
     * 'free_q' acts as a recycled memory pool.
     * 'data_q' acts as the message carrier between threads.
     */
    AudioPool capture_free_q;
    AudioPool capture_data_q;
    AudioPool playback_free_q;
    AudioPool playback_data_q;

    // --- Monadic Hardware & RT Logic ---
    /**
     * @brief Initializes ALSA PCM device with requested stream type and rate.
     * [[nodiscard]]: Forces the caller to handle potential initialization errors.
     * @return std::expected containing the RAII handle or a descriptive SystemError.
     */
    [[nodiscard]] auto setup_pcm(snd_pcm_stream_t type, uint32_t rate) const noexcept
        -> std::expected<UniquePcm, SystemError>;

    /**
     * @brief Hardens a thread by pinning it to a core and setting FIFO priority.
     * Crucial for preventing audio stuttering when the AI containers spike the CPU.
     */
    [[nodiscard]] auto apply_rt_settings(std::thread& thread_handle, int core_id,
                                         int priority) const noexcept
        -> std::expected<void, SystemError>;

    // --- Thread Loops ---
    void mic_loop() noexcept;          // Hardware Capture -> Queue
    void speaker_loop() noexcept;      // Queue -> Hardware Playback
    void zmq_sender_loop() noexcept;   // Queue -> Network (STT)
    void zmq_receiver_loop() noexcept; // Network (TTS) -> Queue

    // --- Functional Helpers (Internal Pipe Logic) ---

    /** @brief Safely extracts a chunk from the pool; returns nullopt if queue is empty. */
    [[nodiscard]] auto pop_chunk(AudioPool& pool) noexcept -> std::optional<AudioChunk*>;

    /** @brief Attempts to push a chunk; returns unexpected(QueueFull) if consumer is lagging. */
    [[nodiscard]] auto push_chunk(AudioPool& pool, AudioChunk* chunk) noexcept
        -> std::expected<void, SystemError>;

    /**
     * @brief Attempts to read exactly Schema::FRAMES_PER_BUFFER from the hardware.
     * This is a blocking call within the thread, but decoupled from other threads via the SPSC
     * queue.
     * @return The same chunk pointer on success, or AlsaOverrun on failure.
     */
    [[nodiscard]] auto read_alsa(snd_pcm_t* pcm, AudioChunk* chunk) const noexcept
        -> std::expected<AudioChunk*, SystemError>;

    /**
     * @brief Handles EPIPE (overrun) or ESTRPIPE (suspend) errors.
     * This is a "Fire and Forget" recovery attempt that resets the ALSA buffer state.
     */
    auto recover_alsa(snd_pcm_t* pcm, int error_code) const noexcept -> void;
};