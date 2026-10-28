#pragma once

#include <cstdint>

class IEncoderInput {
public:
    virtual ~IEncoderInput() = default;

    virtual bool begin() = 0;
    virtual bool end() = 0;

    virtual void update() = 0;
    virtual int32_t consumeDelta() = 0;
};
