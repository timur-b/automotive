#include "Button.h"
#include <Arduino.h>

namespace drivers {

Button::Button(uint8_t pin, ButtonPolarity polarity) : pin_(pin), polarity_(polarity) {}

bool Button::begin() {
    if (initialized_) {
        return true;
    }

    pinMode(pin_, polarity_ == ButtonPolarity::ACTIVE_LOW
                      ? INPUT_PULLUP
                      : INPUT_PULLDOWN);

    initialized_ = true;
    return true;
}

bool Button::end() {
    if (!initialized_) {
        return true;
    }

    // Disable internal pull resistors and return the GPIO to input mode.
    pinMode(pin_, INPUT);

    initialized_ = false;
    return true;
}

bool Button::isPressed() const {
    if (!initialized_) {
        return false;
    }

    const bool high = digitalRead(pin_) == HIGH;

    return polarity_ == ButtonPolarity::ACTIVE_HIGH ? high : !high;
}

} // namespace drivers
