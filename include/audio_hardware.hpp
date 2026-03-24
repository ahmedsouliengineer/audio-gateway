#include <cstdint>
#include <optional>

enum class AlsaError : std::uint8_t
{
    None,
    OpenFailed
};

class AlsaHardware
{
public:
    AlsaHardware() = default;
    auto operator=(const AlsaHardware&) -> AlsaHardware& = delete;
    AlsaHardware(const AlsaHardware&) = delete;

    // Manual destructor is needed to call close()
    ~AlsaHardware();

    auto initialize() -> std::optional<AlsaError>;
    auto close() -> void;

private:
    // I am adding handle_ as a void* to keep the header clean of ALSA types.
    // Initializing to nullptr ensures we don't close random memory.
    void* handle_{nullptr};
};