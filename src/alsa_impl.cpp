#include <alsa/asoundlib.h>
#include <iostream>
#include <optional>

#include "audio_hardware.hpp"

AlsaHardware::~AlsaHardware()
{
    close();
}

auto AlsaHardware::initialize() -> std::optional<AlsaError>
{
    // I am using a local pointer as a bridge.
    // This avoids reinterpret_cast on the member variable address.
    snd_pcm_t* pcm_bridge = nullptr;

    int return_code = snd_pcm_open(&pcm_bridge, "default", SND_PCM_STREAM_CAPTURE, 0);

    if (return_code < 0) {
        std::cerr << "ALSA: Cannot open device default (" << snd_strerror(return_code) << ")\n";
        return AlsaError::OpenFailed;
    }

    // I am storing the successfully opened handle back into our opaque pointer.
    handle_ = static_cast<void*>(pcm_bridge);

    return std::nullopt;
}

auto AlsaHardware::close() -> void
{
    if (handle_ != nullptr) {
        // static_cast is allowed by Core Guidelines for void* conversions.
        snd_pcm_close(static_cast<snd_pcm_t*>(handle_));
        handle_ = nullptr;
    }
}