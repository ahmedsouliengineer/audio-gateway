#include "audio_gateway.hpp"

AudioGateway::AudioGateway(GatewayConfig config, ModuleLogger& log) noexcept
    : config_(std::move(config)), log_(log)
{
    // I am choosing to add this call to resolve the [unusedFunction] error
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