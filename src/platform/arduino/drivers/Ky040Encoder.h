#pragma once

#include <Arduino.h>

#include "input/IEncoderInput.h"
#include "input/QuadratureDecoder.h"

namespace drivers {

struct EncoderConfig {
    uint8_t pinA;
    uint8_t pinB;
    uint8_t transitionsPerStep = 4;
    bool reverseDirection = false;
};

class Ky040Encoder final : public IEncoderInput {
public:
    explicit Ky040Encoder(const EncoderConfig& config);

    void begin() override;
    void update() override;
    int32_t consumeDelta() override;

private:
    EncoderConfig config_;
    QuadratureDecoder decoder_;

    int32_t transitionCount_ = 0;
    int32_t delta_ = 0;
};

} // namespace drivers
