#pragma once

#include <cstdint>

#include "stm32f4xx_hal.h"

namespace drivers {

class Stm32DigitalOutput final {
public:
    Stm32DigitalOutput(
        GPIO_TypeDef* port,
        uint16_t pin,
        bool activeHigh = true
    );

    bool begin();
    bool end();

    void set(bool active);

private:
    static bool enableGpioClock(GPIO_TypeDef* port);

    GPIO_PinState pinState(bool active) const;

    GPIO_TypeDef* port_;
    uint16_t pin_;
    bool activeHigh_;

    bool initialized_ = false;
};

} // namespace drivers
