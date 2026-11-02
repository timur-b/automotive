#pragma once

#include "input/IButton.h"
#include "stm32f4xx_hal.h"

namespace drivers {

class Stm32Button final : public input::IButton {
public:
    Stm32Button(GPIO_TypeDef* port, uint16_t pin)
        : port_(port), pin_(pin) {}

    bool begin() override;
    bool end() override;
    bool isPressed() const override;

private:
    GPIO_TypeDef* port_;
    uint16_t pin_;
    bool initialized_ = false;
};

} // namespace drivers
