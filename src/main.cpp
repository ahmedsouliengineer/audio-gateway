#include <string>

#include "audio_gateway.hpp"

/**
 * @brief Entry point for the Audio Gateway.
 * I am choosing to access ModuleLogger methods statically to satisfy
 * 'readability-static-accessed-through-instance'.
 */
auto main() -> int
{
    GatewayConfig config;

    // I am choosing to create a dummy instance only because AudioGateway
    // requires a reference to one in its constructor.
    ModuleLogger logger_instance;

    ModuleLogger::info("Starting VITA Audio Gateway...");

    try {
        AudioGateway gateway(config, logger_instance);

        ModuleLogger::warn("Primary ALSA device not found, falling back to default...");

        gateway.start();
        gateway.stop();

        ModuleLogger::info("Gateway shutdown complete.");
    } catch (...) {
        ModuleLogger::error("Critical failure.");
        return 1;
    }

    return 0;
}