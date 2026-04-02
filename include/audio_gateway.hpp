#pragma once

#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

/**
 * @brief Minimal error set with explicit size.
 */
enum class SystemError : std::uint8_t
{
    GenericError
};

struct GatewayConfig
{
    std::string device_name{"default"};
};

/**
 * @brief Static-only logger to satisfy instance-access warnings.
 */
class ModuleLogger
{
public:
    static void info(const std::string& msg)
    {
        std::cout << "[INFO] " << msg << '\n';
    }
};

class AudioGateway
{
public:
    explicit AudioGateway(GatewayConfig config, ModuleLogger& log) noexcept;
    ~AudioGateway();

    // Deleted copies to satisfy rule-of-three/five
    AudioGateway(const AudioGateway&) = delete;
    auto operator=(const AudioGateway&) -> AudioGateway& = delete;

    auto start() -> void;
    auto stop() -> void;

private:
    auto worker_loop() noexcept -> void;

    GatewayConfig config_;
    ModuleLogger& log_;
    std::atomic<bool> running_{false};
    std::vector<std::thread> workers_;
};