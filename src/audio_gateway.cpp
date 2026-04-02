#pragma once

#include <alsa/asoundlib.h>
#include <array>
#include <atomic>
#include <cstdint>
#include <expected>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

/**
 * I am choosing 'std::uint8_t' as the base type to satisfy 'performance-enum-size'.
 */
enum class SystemError : std::uint8_t
{
    AlsaOpenFailed,
    AlsaOverrun,
    QueueFull
};

using UniquePcm = std::unique_ptr<snd_pcm_t, void (*)(snd_pcm_t*)>;

namespace Schema
{
inline constexpr size_t k_pool_capacity = 64;
inline constexpr int k_thread_nap_ms = 10;
// I am choosing named constants to resolve 'cppcoreguidelines-avoid-magic-numbers'
inline constexpr uint32_t k_default_sample_rate = 48000;
inline constexpr size_t k_samples_per_chunk = 480;
} // namespace Schema

struct GatewayConfig
{
    std::string capture_device{"default"};
    std::string playback_device{"default"};
    uint32_t sample_rate{Schema::k_default_sample_rate};
};

struct AudioChunk
{
    // I am choosing std::array to satisfy 'cppcoreguidelines-avoid-c-arrays'
    std::array<float, Schema::k_samples_per_chunk> data{};
};

using AudioPool = std::vector<AudioChunk*>;

class ModuleLogger
{
public:
    static void info(const std::string& msg)
    {
        std::cout << "[INFO] " << msg << '\n';
    }
    static void error(const std::string& msg)
    {
        std::cerr << "[ERROR] " << msg << '\n';
    }
};

class AudioGateway
{
public:
    explicit AudioGateway(GatewayConfig config, ModuleLogger& log) noexcept;
    ~AudioGateway();

    AudioGateway(const AudioGateway&) = delete;
    auto operator=(const AudioGateway&) -> AudioGateway& = delete;

    // I am choosing trailing return types to satisfy 'modernize-use-trailing-return-type'
    auto start() -> void;
    auto stop() -> void;

private:
    auto mic_loop() noexcept -> void;
    auto speaker_loop() noexcept -> void;
    auto zmq_sender_loop() noexcept -> void;
    auto zmq_receiver_loop() noexcept -> void;

    [[nodiscard]] static auto pop_chunk(AudioPool& pool) noexcept -> std::optional<AudioChunk*>;
    [[nodiscard]] static auto push_chunk(AudioPool& pool, AudioChunk* chunk) noexcept
        -> std::expected<void, SystemError>;
    [[nodiscard]] static auto setup_pcm(snd_pcm_stream_t type, uint32_t rate) noexcept
        -> std::expected<UniquePcm, SystemError>;
    [[nodiscard]] static auto read_alsa(snd_pcm_t* pcm, AudioChunk* chunk) noexcept
        -> std::expected<AudioChunk*, SystemError>;
    static auto recover_alsa(snd_pcm_t* pcm, int error_code) noexcept -> void;
    [[nodiscard]] static auto apply_rt_settings(std::thread& thread_handle, int core_id,
                                                int priority) noexcept
        -> std::expected<void, SystemError>;

    GatewayConfig config_;
    ModuleLogger& log_;
    std::atomic<bool> running_{false};
    std::vector<std::thread> workers_;
    AudioPool capture_free_q;
    AudioPool playback_free_q;
};