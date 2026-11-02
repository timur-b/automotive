#pragma once

#include "Stm32PwmTimer.h"
#include "output/IPwmLed.h"

#include <cstdint>

namespace drivers {

class Stm32PwmLed final : public output::IPwmLed {
public:
    Stm32PwmLed(Stm32PwmTimer& timer,
                uint32_t channel,
                GPIO_TypeDef* port,
                uint16_t pin,
                uint8_t alternateFunction);

    bool begin() override;
    bool end() override;

    void setBrightness(uint16_t brightness) override;

private:
    Stm32PwmTimer& timer_;
    uint32_t channel_;

    GPIO_TypeDef* port_;
    uint16_t pin_;
    uint8_t alternateFunction_;

    bool initialized_ = false;
};

} // namespace drivers
