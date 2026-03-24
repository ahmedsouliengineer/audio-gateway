#include <iostream>

#include "audio_hardware.hpp"

AlsaHardware::AlsaHardware(std::string device_name) : device_name_(std::move(device_name))
{}

AlsaHardware::~AlsaHardware()
{
    close();
}

std::optional<AlsaError> AlsaHardware::initialize()
{
    // I am attempting to open the PCM device for capture.
    int err = snd_pcm_open(&handle_, device_name_.c_str(), SND_PCM_STREAM_CAPTURE, 0);

    if (err < 0) {
        std::cerr << "ALSA: Cannot open device " << device_name_ << " (" << snd_strerror(err)
                  << ")\n";
        return AlsaError::OpenFailed;
    }

    // Success: Return nothing (nullopt)
    return std::nullopt;
}

void AlsaHardware::close()
{
    if (handle_) {
        snd_pcm_close(handle_);
        handle_ = nullptr;
    }
}