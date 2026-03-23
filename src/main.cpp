#include "audio_hardware.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // I am targeting the 'default' ALSA device.
    AlsaHardware hw("default");

    std::cout << "Audio Gateway: Initializing hardware...\n";
    
    auto result = hw.initialize();
    if (result.has_value()) {
        std::cerr << "Audio Gateway: Critical Failure during startup.\n";
        return 1;
    }

    std::cout << "Audio Gateway: Hardware online. Starting capture loop...\n";

    // I am simulating a running service for 5 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Audio Gateway: Shutting down safely.\n";
    return 0;
}