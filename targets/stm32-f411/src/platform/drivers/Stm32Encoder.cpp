#include "platform/drivers/Stm32Encoder.h"

namespace drivers {

Stm32Encoder::Stm32Encoder(const Config& config)
    : config_(config) {}

bool Stm32Encoder::enableGpioClock(GPIO_TypeDef* port) {
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else {
        return false;
    }

    return true;
}

bool Stm32Encoder::begin() {
    if (initialized_) {
        return true;
    }

    if (config_.portA == nullptr ||
        config_.portB == nullptr ||
        config_.pinA == 0 ||
        config_.pinB == 0 ||
        config_.transitionsPerStep == 0) {
        return false;
    }

    if (config_.portA == config_.portB &&
        (config_.pinA & config_.pinB) != 0) {
        return false;
    }

    if (!enableGpioClock(config_.portA) ||
        !enableGpioClock(config_.portB)) {
        return false;
    }

    GPIO_InitTypeDef gpio{};

    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    gpio.Pin = config_.pinA;
    HAL_GPIO_Init(config_.portA, &gpio);

    gpio.Pin = config_.pinB;
    HAL_GPIO_Init(config_.portB, &gpio);

    const bool a =
        HAL_GPIO_ReadPin(config_.portA, config_.pinA) ==
        GPIO_PIN_SET;

    const bool b =
        HAL_GPIO_ReadPin(config_.portB, config_.pinB) ==
        GPIO_PIN_SET;

    decoder_.reset(a, b);

    transitionCount_ = 0;
    delta_ = 0;

    initialized_ = true;
    return true;
}

bool Stm32Encoder::end() {
    if (!initialized_) {
        return true;
    }

    HAL_GPIO_DeInit(config_.portA, config_.pinA);
    HAL_GPIO_DeInit(config_.portB, config_.pinB);

    transitionCount_ = 0;
    delta_ = 0;

    initialized_ = false;
    return true;
}

void Stm32Encoder::update() {
    if (!initialized_) {
        return;
    }

    const bool a =
        HAL_GPIO_ReadPin(config_.portA, config_.pinA) ==
        GPIO_PIN_SET;

    const bool b =
        HAL_GPIO_ReadPin(config_.portB, config_.pinB) ==
        GPIO_PIN_SET;

    int8_t transition = decoder_.update(a, b);

    if (config_.reverseDirection) {
        transition = -transition;
    }

    transitionCount_ += transition;

    const int32_t threshold = config_.transitionsPerStep;

    if (transitionCount_ >= threshold) {
        ++delta_;
        transitionCount_ -= threshold;
    } else if (transitionCount_ <= -threshold) {
        --delta_;
        transitionCount_ += threshold;
    }
}

int32_t Stm32Encoder::consumeDelta() {
    const int32_t result = delta_;
    delta_ = 0;
    return result;
}

} // namespace drivers
