#include <cstddef>
#include <cstdint>

extern "C" auto LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) -> int
{
    (void) Data;
    (void) Size;
    // I am choosing to leave this empty for now so the build passes.
    // This satisfies the linker's need for an entry point.
    return 0;
}