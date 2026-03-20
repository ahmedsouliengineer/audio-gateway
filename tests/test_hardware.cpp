#include <gtest/gtest.h>
#include "audio_hardware.h"
#include <iostream>

class MockAudioHardware : public AudioHardware {
public:
    std::expected<DeviceInfo, AudioError> probe_default_device() override {
        return std::unexpected(AudioError::Busy);
    }
};

TEST(AudioHardwareTest, HandlesBusyDeviceCorrectly) {
    MockAudioHardware mock;
    auto result = mock.probe_default_device();

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), AudioError::Busy);
}

// I removed the manual print here; GTest handles the output for us.