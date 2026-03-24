#include <gtest/gtest.h>

#include "audio_hardware.hpp"

// I am testing that the class can be initialized and closed without crashing.
TEST(HardwareTest, LifecycleTest)
{
    // I am creating an instance using the default constructor.
    AlsaHardware audio_hardware;

    // I am attempting to initialize the hardware.
    auto result = audio_hardware.initialize();

    // I am explicitly calling close to verify the teardown logic.
    audio_hardware.close();

    SUCCEED();
}

auto main(int argc, char** argv) -> int
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}