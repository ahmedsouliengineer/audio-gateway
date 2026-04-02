#include "audio_gateway.hpp"

// I am choosing a trailing return type to satisfy modernize-use-trailing-return-type.
auto main() -> int
{
    GatewayConfig config;
    ModuleLogger logger;

    try {
        AudioGateway gateway(config, logger);
        gateway.start();
        gateway.stop();
    } catch (...) {
        // I am choosing to catch everything to prevent unhandled exceptions at the boundary.
        return 1;
    }

    return 0;
}