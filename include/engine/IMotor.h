#pragma once

#include <cstdint>

class IMotor {
public:
    virtual ~IMotor() = default;

    virtual bool begin() = 0;
    virtual bool end() = 0;

    virtual void setPower(uint8_t percent) = 0;
    virtual void stop() = 0;
};
