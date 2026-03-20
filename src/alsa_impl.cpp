#include "audio_hardware.h"
#include <alsa/asoundlib.h>

std::expected<DeviceInfo, AudioError> AudioHardware::probe_default_device() {
    snd_pcm_t *handle;
    
    // I am attempting to open the default device in playback mode.
    int rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

    if (rc < 0) {
        if (rc == -EBUSY) return std::unexpected(AudioError::Busy);
        if (rc == -EACCES) return std::unexpected(AudioError::PermissionDenied);
        return std::unexpected(AudioError::DeviceNotFound);
    }

    DeviceInfo info;
    info.name = snd_pcm_name(handle);
    info.card_index = 0; // Simplified for Phase 1

    snd_pcm_close(handle);
    return info;
}