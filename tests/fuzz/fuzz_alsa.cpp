#include <cstddef> // Use C++ headers instead of .h
#include <cstdint>
#include <span>
#include <vector>

// I am using the trailing return type for the fuzzer entry point.
extern "C" auto LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) -> int
{
    // I am adding braces to satisfy 'readability-braces-around-statements'.
    if (Size < 1) {
        return 0;
    }

    // I am using std::span or direct vector initialization to avoid pointer arithmetic.
    // Clang-tidy hates 'Data + Size'.
    std::vector<uint8_t> mock_buffer(std::span(Data, Size).begin(), std::span(Data, Size).end());

    return 0;
}