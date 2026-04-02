#include "audio_gateway.hpp"

auto main() -> int
{
    GatewayConfig config;
    ModuleLogger logger;

    try {
        AudioGateway gateway(config, logger);
        gateway.start();
        gateway.stop();
    } catch (...) {
        return 1;
    }

    return 0;
}