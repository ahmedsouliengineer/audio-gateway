#include <gtest/gtest.h>
#include "audio_hardware.hpp"

// I am testing that the class can be instantiated without crashing.
TEST(HardwareTest, LifecycleTest) {
    AlsaHardware hw("default");
    // We don't check for success here as the CI environment 
    // might not have a physical sound card mapped.
    auto err = hw.initialize();
    SUCCEED(); 
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}