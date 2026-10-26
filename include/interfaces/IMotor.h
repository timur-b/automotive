#pragma once

#include <cstdint>

class IMotor {
public:
    virtual ~IMotor() = default;

    virtual void setPower(uint8_t percent) = 0;
    virtual void stop() = 0;
};
