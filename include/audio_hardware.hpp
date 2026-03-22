#pragma once
#include <alsa/asoundlib.h>
#include <string>
#include <expected>

enum class AlsaError { OpenFailed, ConfigFailed };

class AlsaHardware {
public:
    explicit AlsaHardware(std::string device_name) : name_(std::move(device_name)) {}
    ~AlsaHardware() { close(); }

    std::expected<void, AlsaError> open_capture();
    void close();
    bool is_open() const { return handle_ != nullptr; }

private:
    std::string name_;
    snd_pcm_t* handle_ = nullptr;
};