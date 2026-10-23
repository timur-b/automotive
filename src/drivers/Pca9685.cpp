#include "drivers/Pca9685.h"

namespace drivers {

Pca9685::Pca9685(TwoWire& wire, uint8_t address, float frequencyHz)
    : driver_(address, wire), frequencyHz_(frequencyHz) {}

bool Pca9685::begin() {
    if (initialized_) {
        return true;
    }
    if (!(frequencyHz_ >= 40.0f && frequencyHz_ <= 1000.0f)) {
        return false;
    }
    if (!driver_.begin()) {
        return false;
    }
    driver_.setOutputMode(true);
    driver_.setPWMFreq(frequencyHz_);

    // Clear all outputs once, before individual LEDs are initialized.
    for (uint8_t channel = 0; channel < 16; ++channel) {
        if (driver_.setPWM(channel, 0, 4096) != 0) {
            return false;
        }
    }
    initialized_ = true;
    return true;
}

bool Pca9685::isInitialized() const {
    return initialized_;
}

bool Pca9685::write(uint8_t channel, uint16_t duty) {
    if (!initialized_ || channel >= 16 || duty > 4095) {
        return false;
    }
    if (duty == 0) {
        return driver_.setPWM(channel, 0, 4096) == 0;
    }
    if (duty == 4095) {
        return driver_.setPWM(channel, 4096, 0) == 0;
    }
    return driver_.setPWM(channel, 0, duty) == 0;
}

} // namespace drivers
