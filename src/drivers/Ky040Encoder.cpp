#include "drivers/Ky040Encoder.h"

namespace drivers {

Ky040Encoder::Ky040Encoder(const EncoderConfig& config)
    : config_(config) {
}

void Ky040Encoder::begin() {
    pinMode(config_.pinA, INPUT_PULLUP);
    pinMode(config_.pinB, INPUT_PULLUP);

    decoder_.reset(
        digitalRead(config_.pinA),
        digitalRead(config_.pinB)
    );
}

void Ky040Encoder::update() {
    const bool a = digitalRead(config_.pinA);
    const bool b = digitalRead(config_.pinB);

    int8_t transition = decoder_.update(a, b);

    if (config_.reverseDirection) {
        transition = -transition;
    }

    transitionCount_ += transition;

    if (transitionCount_ >= config_.transitionsPerStep) {
        ++delta_;
        transitionCount_ -= config_.transitionsPerStep;
    } else if (transitionCount_ <=
               -static_cast<int32_t>(config_.transitionsPerStep)) {
        --delta_;
        transitionCount_ += config_.transitionsPerStep;
    }
}

int32_t Ky040Encoder::consumeDelta() {
    const int32_t result = delta_;
    delta_ = 0;
    return result;
}

} // namespace drivers
