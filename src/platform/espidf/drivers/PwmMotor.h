#pragma once

#include <cstdint>

#include "engine/IMotor.h"
#include "LedcResourceManager.h"

namespace drivers {

class PwmMotor final : public IMotor {
public:
    PwmMotor(LedcResourceManager& ledc, uint8_t pin, uint8_t channel);

    bool begin();

    void setPower(uint8_t percent) override;
    void stop() override;

private:
    static constexpr uint32_t PWM_FREQUENCY = 5000;
    static constexpr uint8_t PWM_RESOLUTION = 10;
    static constexpr uint32_t MAX_DUTY =
        (1U << PWM_RESOLUTION) - 1U;

    LedcResourceManager& ledc_;
    uint8_t pin_;
    uint8_t channel_;
    bool initialized_ = false;
};

} // namespace drivers
