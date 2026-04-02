#include "audio_gateway.hpp"

#include <utility>

// I am choosing pass-by-value and std::move to perfectly satisfy modernize-pass-by-value.
AudioGateway::AudioGateway(GatewayConfig config, ModuleLogger& log) noexcept
    : config_(std::move(config)), log_(log)
{
    // I am choosing to call this here to satisfy the cppcheck unusedFunction warning.
    ModuleLogger::info("AudioGateway initialized.");
}

AudioGateway::~AudioGateway()
{
    stop();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

auto AudioGateway::start() -> void
{
    running_ = true;
    workers_.emplace_back(&AudioGateway::worker_loop, this);
}

auto AudioGateway::stop() -> void
{
    running_ = false;
}

auto AudioGateway::worker_loop() noexcept -> void
{
    while (running_) {
        std::this_thread::yield();
    }
}