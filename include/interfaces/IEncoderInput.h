#pragma once

#include <cstdint>

class IEncoderInput {
public:
    virtual ~IEncoderInput() = default;

    virtual int32_t consumeDelta() = 0;
};
