#pragma once
#include <cstdint>
#include <string>

/**
 * @brief ARCHITECTURAL SCHEMA
 * These values define the memory footprint and real-time behavior.
 * Changing these requires a recompilation of the Docker image.
 * This guarantees "Zero-Allocation" stability on your 4GB Radxa.
 */
namespace Schema
{
// Memory Pool Scaling
constexpr size_t k_pool_capacity = 16;       // 1.6 seconds of total audio "flight" time
constexpr size_t k_frames_per_buffer = 1600; // 100ms chunks (standard for STT/VAD)

// Audio Engineering Constants
constexpr uint32_t k_mic_sample_rate = 16000; // Standard for Moonshine/Whisper
constexpr uint32_t k_spk_sample_rate = 24000; // Native for Kokoro TTS

// Real-Time Scheduling (Higher = More Priority)
constexpr int k_priority_critical = 85; // For Mic/Speaker threads
constexpr int k_priority_high = 80;     // For ZMQ Network threads

constexpr int k_thread_nap_ms = 10; // I am adding this to fix the 'no_member' diagnostic
} // namespace Schema

/**
 * @brief INTERNAL DEFAULTS
 * Used to avoid 'magic number' diagnostics in the GatewayConfig struct.
 */
namespace Defaults
{
constexpr int k_zmq_hwm = 600;
constexpr int k_zmq_linger_ms = 100;
constexpr int k_core_mic = 0;
constexpr int k_core_spk = 1;
constexpr int k_core_net = 2;
} // namespace Defaults

/**
 * @brief ENVIRONMENTAL CONFIGURATION
 * These values are injected at runtime (via Config File or Env Vars).
 * They do NOT affect the core architecture or memory safety.
 */
struct GatewayConfig
{
    // Hardware Addressing
    std::string alsa_mic_device = "default";
    std::string alsa_spk_device = "default";

    // Network Topology
    std::string stt_endpoint = "tcp://stt-node:5555";
    std::string tts_endpoint = "tcp://*:5558";

    // Fault Tolerance (ZMQ Internal Buffering)
    int zmq_hwm = Defaults::k_zmq_hwm;             // 60-second Deep Buffer safety net
    int zmq_linger_ms = Defaults::k_zmq_linger_ms; // Prevent hanging on container shutdown

    // CPU Pinning (Adjust based on Radxa core count)
    int core_mic = Defaults::k_core_mic;
    int core_spk = Defaults::k_core_spk;
    int core_net = Defaults::k_core_net;
};