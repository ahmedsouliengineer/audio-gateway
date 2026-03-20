#include "audio_hardware.h"
#include <iostream> // Swapped from <print>

int main() {
    AudioHardware hw;
    auto result = hw.probe_default_device();

    if (!result) {
        // I am using standard output for now since GCC 13 hasn't 
        // fully cooked the <print> header yet.
        std::cerr << "Hardware Probe Failed: Error Code " << (int)result.error() << std::endl;
        return 1;
    }

    std::cout << "Successfully initialized: " << result->name << std::endl;
    return 0;
}