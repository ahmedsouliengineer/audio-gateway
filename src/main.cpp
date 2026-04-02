#include <iostream>

#include "audio_gateway.hpp"

// Minimal logger stub to satisfy the constructor
class ModuleLogger
{
public:
    static void info(const std::string& msg)
    {
        std::cout << "[INFO] " << msg << "\n";
    }
    static void error(const std::string& msg)
    {
        std::cerr << "[ERROR] " << msg << "\n";
    }
    static void warn(const std::string& msg)
    {
        std::cout << "[WARN] " << msg << "\n";
    }
};

auto main() -> int
{
    ModuleLogger logger;
    GatewayConfig config; // Uses defaults from config.hpp

    std::cout << "Starting VITA Audio Gateway..." << "\n";

    AudioGateway gateway(config, logger);
    gateway.start();

    // For now, we just verify it starts and stops
    gateway.stop();

    return 0;
}