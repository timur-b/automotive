#pragma once

#include <cstdint>

#include "Stm32PwmTimer.h"

namespace drivers {

class Stm32PwmMotor final {
public:
    struct Config {
        uint8_t startupDutyPercent = 27;
        uint32_t startupDurationMs = 100;
    };

    Stm32PwmMotor(
        Stm32PwmTimer& timer,
        uint32_t channel,
        GPIO_TypeDef* port,
        uint16_t pin,
        uint8_t alternateFunction,
        const Config& config
    );

    bool begin();
    bool end();

    void setDutyPercent(uint8_t percent);
    void update(uint32_t now);

private:
    enum class State : uint8_t {
        Stopped,
        Starting,
        Running
    };

    void applyDuty(uint8_t percent);

    Stm32PwmTimer& timer_;
    uint32_t channel_;

    GPIO_TypeDef* port_;
    uint16_t pin_;
    uint8_t alternateFunction_;

    Config config_;

    State state_ = State::Stopped;

    uint8_t requestedDuty_ = 0;
    uint32_t startupStartedAt_ = 0;

    bool initialized_ = false;
};

} // namespace drivers
