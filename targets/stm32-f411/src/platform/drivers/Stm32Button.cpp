#include "Stm32Button.h"

namespace drivers {

bool Stm32Button::begin() {
    if (initialized_) {
        return true;
    }

    if (port_ != GPIOA) {
        return false;
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio{};
    gpio.Pin = pin_;
    // gpio.Mode = GPIO_MODE_INPUT;
    gpio.Mode = mode_ == InputMode::Interrupt
                ? GPIO_MODE_IT_RISING_FALLING
                : GPIO_MODE_INPUT;
    gpio.Pull = polarity_ == Polarity::ActiveHigh
                    ? GPIO_PULLDOWN
                    : GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port_, &gpio);

    initialized_ = true;
    return true;
}

bool Stm32Button::end() {
    if (!initialized_) {
        return true;
    }

    HAL_GPIO_DeInit(port_, pin_);
    initialized_ = false;
    return true;
}

bool Stm32Button::isPressed() const {
    if (!initialized_) {
        return false;
    }

    const GPIO_PinState state =
        HAL_GPIO_ReadPin(port_, pin_);

    return polarity_ == Polarity::ActiveHigh
               ? state == GPIO_PIN_SET
               : state == GPIO_PIN_RESET;
}

} // namespace drivers
