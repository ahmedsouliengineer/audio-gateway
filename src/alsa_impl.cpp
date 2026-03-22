#include "audio_hardware.hpp"

std::expected<void, AlsaError> AlsaHardware::open_capture() {
    if (handle_) return {}; 

    int rc = snd_pcm_open(&handle_, name_.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        handle_ = nullptr;
        return std::unexpected(AlsaError::OpenFailed);
    }
    return {};
}

void AlsaHardware::close() {
    if (handle_) {
        snd_pcm_close(handle_);
        handle_ = nullptr;
    }
}