#include "drivers/Esp32PwmMotor.h"

namespace drivers {

Esp32PwmMotor::Esp32PwmMotor(uint8_t pin, uint8_t channel)
    : pin_(pin),
      channel_(channel) {
}

bool Esp32PwmMotor::begin() {
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);

    if (ledcSetup(channel_, PWM_FREQUENCY, PWM_RESOLUTION) <= 0) {
        return false;
    }

    ledcAttachPin(pin_, channel_);
    stop();

    return true;
}

void Esp32PwmMotor::setPower(uint8_t percent) {
    if (percent > 100) {
        percent = 100;
    }

    const uint32_t duty =
        (static_cast<uint32_t>(percent) * MAX_DUTY) / 100U;

    ledcWrite(channel_, duty);
}

void Esp32PwmMotor::stop() {
    ledcWrite(channel_, 0);
}

} // namespace drivers
