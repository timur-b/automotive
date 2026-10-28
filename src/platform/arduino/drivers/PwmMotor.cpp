#include "PwmMotor.h"

namespace drivers {

PwmMotor::PwmMotor(uint8_t pin, uint8_t channel)
    : pin_(pin),
      channel_(channel) {
}

PwmMotor::~PwmMotor() {
    end();
}

bool PwmMotor::begin() {
    if (initialized_) {
        return true;
    }

    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);

    if (ledcSetup(channel_, PWM_FREQUENCY, PWM_RESOLUTION) <= 0) {
        return false;
    }

    ledcAttachPin(pin_, channel_);

    initialized_ = true;
    stop();

    return true;
}

bool PwmMotor::end() {
    if (!initialized_) {
        return true;
    }

    // Stop the motor before detaching the PWM output.
    stop();

    // Disconnect the GPIO from the LEDC peripheral.
    ledcDetachPin(pin_);

    // Keep the motor control signal LOW after shutdown.
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);

    initialized_ = false;
    return true;
}

void PwmMotor::setPower(uint8_t percent) {
    if (!initialized_) {
        return;
    }

    if (percent > 100) {
        percent = 100;
    }

    const uint32_t duty =
        (static_cast<uint32_t>(percent) * MAX_DUTY) / 100U;

    ledcWrite(channel_, duty);
}

void PwmMotor::stop() {
    if (!initialized_) {
        return;
    }

    ledcWrite(channel_, 0);
}

} // namespace drivers
