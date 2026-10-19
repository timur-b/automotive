#pragma once

#include <cstdint>

namespace input {

class ILightSensor {
public:
    virtual ~ILightSensor() = default;

    virtual void begin() = 0;
    virtual uint16_t readRaw() = 0;
};

} // namespace input
