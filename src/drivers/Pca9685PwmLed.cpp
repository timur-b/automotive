#include "drivers/Pca9685PwmLed.h"

namespace drivers {

Pca9685PwmLed::Pca9685PwmLed(Pca9685& controller, uint8_t channel, bool activeHigh)
    : controller_(controller), channel_(channel), activeHigh_(activeHigh) {}

bool Pca9685PwmLed::begin() {
    if (initialized_) {
        return true;
    }
    if (channel_ >= 16 || !controller_.isInitialized()) {
        return false;
    }
    const uint16_t offDuty = activeHigh_ ? 0 : 4095;
    if (!controller_.write(channel_, offDuty)) {
        return false;
    }
    lastDuty_ = offDuty;
    hasLastDuty_ = true;
    initialized_ = true;
    return true;
}

void Pca9685PwmLed::setBrightness(uint16_t brightness) {
    if (!initialized_) {
        return;
    }
    uint16_t duty = static_cast<uint16_t>(
        (static_cast<uint32_t>(brightness) * 4095UL + 32767UL) / 65535UL);
    if (!activeHigh_) {
        duty = 4095 - duty;
    }
    // Avoid redundant I2C transfers; retry failed writes on the next update.
    if (hasLastDuty_ && duty == lastDuty_) {
        return;
    }
    if (controller_.write(channel_, duty)) {
        lastDuty_ = duty;
        hasLastDuty_ = true;
    }
}

} // namespace drivers
