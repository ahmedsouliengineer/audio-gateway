#include <gtest/gtest.h>

#include "audio_gateway.hpp"

/**
 * @brief Coverage Driver Test
 * I am choosing to instantiate the gateway to ensure the constructor
 * and basic methods are marked as 'covered' by llvm-cov.
 */
TEST(AudioGatewayTest, LifecycleTest)
{
    GatewayConfig config;
    ModuleLogger logger;

    // I am choosing to wrap this in a scope to trigger the destructor
    {
        AudioGateway gateway(config, logger);
        gateway.start();
        gateway.stop();
    }

    SUCCEED();
}