#include "drivers/Esp32PwmLed.h"

#include <Arduino.h>

namespace drivers {

Esp32PwmLed::Esp32PwmLed(uint8_t pin, uint8_t channel, uint32_t frequencyHz, uint8_t resolutionBits,
                         bool activeHigh)
    : pin_(pin), channel_(channel), frequencyHz_(frequencyHz), resolutionBits_(resolutionBits),
      activeHigh_(activeHigh) {}

bool Esp32PwmLed::begin() {
    if (initialized_) {
        return true;
    }

    if (resolutionBits_ < 1 || resolutionBits_ > 14 || channel_ >= 8) {
        return false;
    }

    // Configure the LEDC channel.
    const double actualFrequency = ledcSetup(channel_, frequencyHz_, resolutionBits_);

    if (actualFrequency <= 0) {
        return false;
    }

    // Attach the GPIO pin to the configured channel.
    ledcAttachPin(pin_, channel_);

    initialized_ = true;
    setBrightness(0);

    return true;
}

void Esp32PwmLed::setBrightness(uint16_t brightness) {
    if (!initialized_) {
        return;
    }

    const uint32_t maxDuty = (1UL << resolutionBits_) - 1UL;

    uint32_t duty = (static_cast<uint32_t>(brightness) * maxDuty + 32767UL) / 65535UL;

    if (!activeHigh_) {
        duty = maxDuty - duty;
    }

    // Arduino-ESP32 2.x expects a channel, not a GPIO pin.
    ledcWrite(channel_, duty);
}

} // namespace drivers
