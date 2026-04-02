#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// I am choosing to keep this minimal enum to establish the pattern for future errors.
enum class SystemError : std::uint8_t
{
    GenericError
};

struct GatewayConfig
{
    std::string device_name{"default"};
};

class ModuleLogger
{
public:
    // I am choosing to use a static method to avoid instance-access linter warnings.
    static auto info(const std::string& msg) -> void
    {
        std::cout << "[INFO] " << msg << '\n';
    }
};

class AudioGateway
{
public:
    explicit AudioGateway(GatewayConfig config, ModuleLogger& log) noexcept;
    ~AudioGateway();

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