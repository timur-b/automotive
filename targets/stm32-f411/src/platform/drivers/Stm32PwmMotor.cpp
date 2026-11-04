#include "platform/drivers/Stm32PwmMotor.h"

namespace drivers {

Stm32PwmMotor::Stm32PwmMotor(
    Stm32PwmTimer& timer,
    uint32_t channel,
    GPIO_TypeDef* port,
    uint16_t pin,
    uint8_t alternateFunction,
    const Config& config)
    : timer_(timer),
      channel_(channel),
      port_(port),
      pin_(pin),
      alternateFunction_(alternateFunction),
      config_(config) {}

bool Stm32PwmMotor::begin() {
    if (initialized_) {
        return true;
    }

    TIM_HandleTypeDef* handle = timer_.handle();

    if (handle == nullptr ||
        handle->Instance != TIM2 ||
        channel_ != TIM_CHANNEL_3 ||
        port_ != GPIOB ||
        pin_ != GPIO_PIN_10 ||
        alternateFunction_ != GPIO_AF1_TIM2) {
        return false;
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Keep the transistor switched off before configuring PWM.
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);

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
    config.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(
            handle, &config, channel_) != HAL_OK) {
        HAL_GPIO_DeInit(port_, pin_);
        return false;
    }

    if (HAL_TIM_PWM_Start(handle, channel_) != HAL_OK) {
        HAL_GPIO_DeInit(port_, pin_);
        return false;
    }

    requestedDuty_ = 0;
    startupStartedAt_ = 0;
    state_ = State::Stopped;

    initialized_ = true;
    return true;
}

void Stm32PwmMotor::setDutyPercent(uint8_t percent) {
    if (!initialized_) {
        return;
    }

    requestedDuty_ = percent > 100 ? 100 : percent;

    if (requestedDuty_ == 0) {
        state_ = State::Stopped;
        applyDuty(0);
    }
}

bool Stm32PwmMotor::end() {
    if (!initialized_) {
        return true;
    }

    TIM_HandleTypeDef* handle = timer_.handle();

    if (handle == nullptr) {
        return false;
    }

    // Disable the motor before stopping PWM.
    requestedDuty_ = 0;
    state_ = State::Stopped;
    applyDuty(0);

    if (HAL_TIM_PWM_Stop(handle, channel_) != HAL_OK) {
        return false;
    }

    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);

    GPIO_InitTypeDef gpio{};
    gpio.Pin = pin_;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port_, &gpio);

    initialized_ = false;
    return true;
}

void Stm32PwmMotor::applyDuty(uint8_t percent) {
    TIM_HandleTypeDef* handle = timer_.handle();

    if (handle == nullptr) {
        return;
    }

    if (percent > 100) {
        percent = 100;
    }

    const uint32_t duty =
        ((timer_.period() + 1U) *
         static_cast<uint32_t>(percent)) / 100U;

    __HAL_TIM_SET_COMPARE(handle, channel_, duty);
}

void Stm32PwmMotor::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    switch (state_) {
        case State::Stopped:
            if (requestedDuty_ > 0) {
                startupStartedAt_ = now;
                state_ = State::Starting;

                applyDuty(config_.startupDutyPercent);
            }
            break;

        case State::Starting:
            if (static_cast<uint32_t>(
                    now - startupStartedAt_) >=
                config_.startupDurationMs) {

                state_ = State::Running;
                applyDuty(requestedDuty_);
            }
            break;

        case State::Running:
            applyDuty(requestedDuty_);
            break;
    }
}

} // namespace drivers
