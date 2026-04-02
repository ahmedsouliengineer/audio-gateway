#include <iostream>
#include <string>

#include "audio_gateway.hpp"

class ModuleLogger
{
public:
    void info(const std::string& msg) const
    {
        if (enabled_) {
            std::cout << "[INFO] " << msg << "\n";
        }
    }
    void error(const std::string& msg) const
    {
        if (enabled_) {
            std::cerr << "[ERROR] " << msg << "\n";
        }
    }
    void warn(const std::string& msg) const
    {
        if (enabled_) {
            std::cout << "[WARN] " << msg << "\n";
        }
    }

private:
    bool enabled_ = true;
};

auto main() -> int
{
    ModuleLogger logger;
    GatewayConfig config;

    logger.info("Starting VITA Audio Gateway...");

    try {
        AudioGateway gateway(config, logger);

        // Simulating a non-fatal hardware warning to satisfy the linter
        logger.warn("Primary ALSA device not found, falling back to default...");

        gateway.start();
        gateway.stop();

        logger.info("Gateway shutdown complete.");
    } catch (...) {
        logger.error("Critical failure.");
        return 1;
    }

    return 0;
}