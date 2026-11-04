#include "Stm32PwmLed.h"

namespace drivers {

namespace {

bool isValidPinConfiguration(
    TIM_TypeDef* instance,
    uint32_t channel,
    GPIO_TypeDef* port,
    uint16_t pin,
    uint8_t alternateFunction) {

    if (instance == TIM1 &&
        port == GPIOA &&
        alternateFunction == GPIO_AF1_TIM1) {

        return (channel == TIM_CHANNEL_1 && pin == GPIO_PIN_8) ||
               (channel == TIM_CHANNEL_2 && pin == GPIO_PIN_9) ||
               (channel == TIM_CHANNEL_3 && pin == GPIO_PIN_10) ||
               (channel == TIM_CHANNEL_4 && pin == GPIO_PIN_11);
    }

    if (instance == TIM4 &&
        port == GPIOB &&
        alternateFunction == GPIO_AF2_TIM4) {

        return (channel == TIM_CHANNEL_1 && pin == GPIO_PIN_6) ||
               (channel == TIM_CHANNEL_2 && pin == GPIO_PIN_7);
    }

    return false;
}

bool enableGpioClock(GPIO_TypeDef* port) {
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else {
        return false;
    }

    return true;
}

} // namespace

Stm32PwmLed::Stm32PwmLed(
    Stm32PwmTimer& timer,
    uint32_t channel,
    GPIO_TypeDef* port,
    uint16_t pin,
    uint8_t alternateFunction)
    : timer_(timer),
      channel_(channel),
      port_(port),
      pin_(pin),
      alternateFunction_(alternateFunction) {}

bool Stm32PwmLed::begin() {
    if (initialized_) {
        return true;
    }

    TIM_HandleTypeDef* handle = timer_.handle();

    if (handle == nullptr) {
        return false;
    }

    if (!isValidPinConfiguration(
            handle->Instance,
            channel_,
            port_,
            pin_,
            alternateFunction_)) {
        return false;
    }

    if (!enableGpioClock(port_)) {
        return false;
    }

    GPIO_InitTypeDef gpio{};
    gpio.Pin = pin_;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = alternateFunction_;

    HAL_GPIO_Init(port_, &gpio);

    TIM_OC_InitTypeDef config{};
    config.OCMode = TIM_OCMODE_PWM1;
    config.Pulse = 0;
    config.OCPolarity = TIM_OCPOLARITY_HIGH;
    config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    config.OCFastMode = TIM_OCFAST_DISABLE;
    config.OCIdleState = TIM_OCIDLESTATE_RESET;
    config.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(
            handle, &config, channel_) != HAL_OK) {
        HAL_GPIO_DeInit(port_, pin_);
        return false;
    }

    if (HAL_TIM_PWM_Start(handle, channel_) != HAL_OK) {
        HAL_GPIO_DeInit(port_, pin_);
        return false;
    }

    initialized_ = true;
    return true;
}

bool Stm32PwmLed::end() {
    if (!initialized_) {
        return true;
    }

    TIM_HandleTypeDef* handle = timer_.handle();

    if (handle == nullptr) {
        return false;
    }

    // Turn off the LED before stopping PWM.
    __HAL_TIM_SET_COMPARE(handle, channel_, 0);

    if (HAL_TIM_PWM_Stop(handle, channel_) != HAL_OK) {
        return false;
    }

    HAL_GPIO_DeInit(port_, pin_);

    // Keep the output low after releasing alternate function.
    GPIO_InitTypeDef gpio{};
    gpio.Pin = pin_;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port_, &gpio);
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);

    initialized_ = false;
    return true;
}

void Stm32PwmLed::setBrightness(uint16_t brightness) {
    if (!initialized_) {
        return;
    }

    TIM_HandleTypeDef* handle = timer_.handle();

    if (handle == nullptr) {
        return;
    }

    const uint32_t period = timer_.period();

    // Scale 16-bit brightness to timer duty cycle.
    const uint32_t duty = static_cast<uint32_t>(
        (static_cast<uint64_t>(brightness) *
         (static_cast<uint64_t>(period) + 1U)) / 65535U
    );

    __HAL_TIM_SET_COMPARE(handle, channel_, duty);
}

} // namespace drivers
