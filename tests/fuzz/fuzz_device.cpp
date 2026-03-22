#include "audio_hardware.hpp"
#include <stddef.h>
#include <stdint.h>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    std::string fuzzy_name(reinterpret_cast<const char*>(data), size);

    AlsaHardware hw(fuzzy_name);
    // We are testing if the ALSA C-library or our wrapper 
    // crashes when handled "garbage" strings.
    auto res = hw.open_capture();
    
    if (hw.is_open()) {
        hw.close();
    }

    return 0;
}