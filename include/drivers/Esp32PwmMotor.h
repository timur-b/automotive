#pragma once

#include <Arduino.h>

#include "interfaces/IMotor.h"

namespace drivers {

class Esp32PwmMotor final : public IMotor {
public:
    Esp32PwmMotor(uint8_t pin, uint8_t channel);

    bool begin();

    void setPower(uint8_t percent) override;
    void stop() override;

private:
    static constexpr uint32_t PWM_FREQUENCY = 5000;
    static constexpr uint8_t PWM_RESOLUTION = 10;
    static constexpr uint32_t MAX_DUTY =
        (1U << PWM_RESOLUTION) - 1U;

    uint8_t pin_;
    uint8_t channel_;
};

} // namespace drivers
