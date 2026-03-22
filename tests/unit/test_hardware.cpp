#include <gtest/gtest.h>
#include "audio_hardware.hpp"

TEST(HardwareTest, InitialStateIsClosed) {
    AlsaHardware hw("default");
    EXPECT_FALSE(hw.is_open());
}

TEST(HardwareTest, FailsOnInvalidDevice) {
    // We expect this to fail gracefully without crashing
    AlsaHardware hw("non_existent_device_12345");
    auto res = hw.open_capture();
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(res.error(), AlsaError::OpenFailed);
}