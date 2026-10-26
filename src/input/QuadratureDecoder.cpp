#include "input/QuadratureDecoder.h"

void QuadratureDecoder::reset(bool a, bool b) {
    previous_ = (static_cast<uint8_t>(a) << 1) |
                static_cast<uint8_t>(b);
}

int8_t QuadratureDecoder::update(bool a, bool b) {
    const uint8_t current =
        (static_cast<uint8_t>(a) << 1) |
        static_cast<uint8_t>(b);

    const uint8_t index = (previous_ << 2) | current;

    const int8_t delta = TRANSITIONS[index];

    previous_ = current;

    return delta;
}
