#include "audio_hardware.hpp"
#include <iostream>

int main() {
    AlsaDevice mic("default");
    auto result = mic.open_capture();

    if (result) {
        std::cout << "ALSA Device opened successfully!" << std::endl;
    } else {
        std::cerr << "Failed to open ALSA device (Expected in CI/Docker without hardware access)." << std::endl;
    }

    return 0;
}