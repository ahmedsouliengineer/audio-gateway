#pragma once
#include <expected>
#include <string>
#include <vector>

// I am defining our error codes here so the Junior Devs 
// don't have to guess what a '-1' return code means.
enum class AudioError {
    DeviceNotFound,
    PermissionDenied,
    Busy,
    Unknown
};

struct DeviceInfo {
    std::string name;
    int card_index;
};

// I am putting the hardware logic in a class so we can 
// eventually inherit from it for our Mock objects.
class AudioHardware {
public:
    virtual ~AudioHardware() = default;

    // I am using std::expected to return either the device name 
    // or a specific AudioError.
    virtual std::expected<DeviceInfo, AudioError> probe_default_device();
};