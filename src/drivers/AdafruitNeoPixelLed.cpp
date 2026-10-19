#include "drivers/AdafruitNeoPixelLed.h"

namespace drivers {

AdafruitNeoPixelLed::AdafruitNeoPixelLed(uint8_t pin) : pixel_(1, pin, NEO_GRB + NEO_KHZ800) {}

bool AdafruitNeoPixelLed::begin() {
    if (initialized_) {
        return true;
    }

    pixel_.begin();
    pixel_.clear();
    pixel_.show();

    initialized_ = true;
    return true;
}

void AdafruitNeoPixelLed::setColor(output::RgbColor color) {
    if (!initialized_) {
        return;
    }

    pixel_.setPixelColor(0, pixel_.Color(color.red, color.green, color.blue));

    pixel_.show();
}

} // namespace drivers
