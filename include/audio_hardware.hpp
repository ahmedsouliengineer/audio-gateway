#pragma once

#include <string>
#include <optional>
#include <alsa/asoundlib.h>

// I am using a scoped enum for type safety. 
enum class AlsaError {
    OpenFailed,
    ConfigFailed,
    StreamError
};

class AlsaHardware {
public:
    explicit AlsaHardware(std::string device_name);
    ~AlsaHardware();

    // I am preventing copying to avoid double-closing the ALSA handle.
    AlsaHardware(const AlsaHardware&) = delete;
    AlsaHardware& operator=(const AlsaHardware&) = delete;

    // Returns nullopt on success, or an error code on failure.
    std::optional<AlsaError> initialize();
    void close();

private:
    std::string device_name_;
    snd_pcm_t* handle_{nullptr};
};