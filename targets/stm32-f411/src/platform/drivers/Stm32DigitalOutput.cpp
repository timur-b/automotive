#include "platform/drivers/Stm32DigitalOutput.h"

namespace drivers {

Stm32DigitalOutput::Stm32DigitalOutput(
    GPIO_TypeDef* port,
    uint16_t pin,
    bool activeHigh)
    : port_(port),
      pin_(pin),
      activeHigh_(activeHigh) {}

bool Stm32DigitalOutput::enableGpioClock(GPIO_TypeDef* port) {
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else {
        return false;
    }

    return true;
}

GPIO_PinState Stm32DigitalOutput::pinState(bool active) const {
    const bool high = active == activeHigh_;

    return high ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

bool Stm32DigitalOutput::begin() {
    if (initialized_) {
        return true;
    }

    if (port_ == nullptr ||
        pin_ == 0 ||
        (pin_ & (pin_ - 1U)) != 0) {
        return false;
    }

    if (!enableGpioClock(port_)) {
        return false;
    }

    // Set the inactive output level before enabling output mode.
    HAL_GPIO_WritePin(port_, pin_, pinState(false));

    GPIO_InitTypeDef gpio{};
    gpio.Pin = pin_;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port_, &gpio);

    initialized_ = true;
    return true;
}

void Stm32DigitalOutput::set(bool active) {
    if (!initialized_) {
        return;
    }

    HAL_GPIO_WritePin(port_, pin_, pinState(active));
}

bool Stm32DigitalOutput::end() {
    if (!initialized_) {
        return true;
    }

    // Switch the output off before releasing the GPIO.
    set(false);

    HAL_GPIO_DeInit(port_, pin_);

    initialized_ = false;
    return true;
}

} // namespace drivers
