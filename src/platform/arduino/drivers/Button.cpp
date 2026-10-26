#include "Button.h"
#include <Arduino.h>

namespace drivers {

Button::Button(uint8_t pin, ButtonPolarity polarity) : pin_(pin), polarity_(polarity) {}

void Button::begin() {
    pinMode(pin_, polarity_ == ButtonPolarity::ACTIVE_LOW ? INPUT_PULLUP : INPUT_PULLDOWN);
}

bool Button::isPressed() const {
    const bool high = digitalRead(pin_) == HIGH;

    return polarity_ == ButtonPolarity::ACTIVE_HIGH ? high : !high;
}

} // namespace drivers
