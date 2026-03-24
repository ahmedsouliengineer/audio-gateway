#include <chrono>
#include <iostream>
#include <thread>

#include "audio_hardware.hpp"

// I am using the trailing return type for main to satisfy 'modernize-use-trailing-return-type'.
auto main() -> int
{
    // I am choosing 'audio_hw' to satisfy 'readability-identifier-length' (>= 3 chars).
    // I am using the default constructor as now defined in the header.
    AlsaHardware audio_hw;

    std::cout << "Audio Gateway: Initializing hardware...\n";

    auto result = audio_hw.initialize();
    if (result.has_value()) {
        std::cerr << "Audio Gateway: Critical Failure during startup.\n";
        return 1;
    }

    std::cout << "Audio Gateway: Hardware online. Starting capture loop...\n";

    // I am defining a named constant to satisfy 'cppcoreguidelines-avoid-magic-numbers'.
    constexpr int sleep_duration_seconds = 5;
    std::this_thread::sleep_for(std::chrono::seconds(sleep_duration_seconds));

    std::cout << "Audio Gateway: Shutting down safely.\n";
    return 0;
}