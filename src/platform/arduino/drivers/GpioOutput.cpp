#include "GpioOutput.h"

#include <Arduino.h>

namespace drivers {

GpioOutput::GpioOutput(uint8_t pin, bool activeHigh)
    : pin_(pin), activeHigh_(activeHigh) {}

bool GpioOutput::begin() {
    if (initialized_) {
        return true;
    }

    // Set the inactive level before enabling the output.
    digitalWrite(pin_, activeHigh_ ? LOW : HIGH);
    pinMode(pin_, OUTPUT);

    initialized_ = true;
    return true;
}

bool GpioOutput::end() {
    if (!initialized_) {
        return true;
    }

    set(false);
    pinMode(pin_, INPUT);

    initialized_ = false;
    return true;
}

void GpioOutput::set(bool active) {
    if (!initialized_) {
        return;
    }

    const bool high = active == activeHigh_;
    digitalWrite(pin_, high ? HIGH : LOW);
}

} // namespace drivers
