#include <iostream>

#include "audio_hardware.hpp"

AlsaHardware::AlsaHardware(std::string device_name) : device_name_(std::move(device_name))
{}

AlsaHardware::~AlsaHardware()
{
    close();
}

// I am choosing a trailing return type here to satisfy the modern C++23 style
// required by the clang-tidy 'modernize' check.
auto AlsaHardware::initialize() -> std::optional<AlsaError>
{
    // I am attempting to open the PCM device for capture.
    int err = snd_pcm_open(&handle_, device_name_.c_str(), SND_PCM_STREAM_CAPTURE, 0);

    if (err < 0) {
        std::cerr << "ALSA: Cannot open device " << device_name_ << " (" << snd_strerror(err)
                  << ")\n";
        return AlsaError::OpenFailed;
    }

    return std::nullopt;
}

// I am choosing a trailing return type for consistency across the implementation.
auto AlsaHardware::close() -> void
{
    // I am using an explicit nullptr check here to resolve the
    // 'readability-implicit-bool-conversion' warning.
    if (handle_ != nullptr) {
        snd_pcm_close(handle_);
        handle_ = nullptr;
    }
}