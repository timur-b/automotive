#pragma once

#include "input/IButton.h"
#include "stm32f4xx_hal.h"

namespace drivers {

class Stm32Button final : public input::IButton {
public:
    enum class Polarity {
        ActiveLow,
        ActiveHigh
    };

    Stm32Button(
        GPIO_TypeDef* port,
        uint16_t pin,
        Polarity polarity)
        : port_(port),
          pin_(pin),
          polarity_(polarity) {}

    bool begin() override;
    bool end() override;
    bool isPressed() const override;

private:
    GPIO_TypeDef* port_;
    uint16_t pin_;
    Polarity polarity_;

    bool initialized_ = false;
};

} // namespace drivers
