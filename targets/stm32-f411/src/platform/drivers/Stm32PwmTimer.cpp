#include "Stm32PwmTimer.h"

namespace drivers {

namespace {

bool enableTimerClock(TIM_TypeDef* instance) {
    if (instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    } else if (instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    } else if (instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    } else {
        return false;
    }

    return true;
}

void disableTimerClock(TIM_TypeDef* instance) {
    if (instance == TIM1) {
        __HAL_RCC_TIM1_CLK_DISABLE();
    } else if (instance == TIM2) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    } else if (instance == TIM4) {
        __HAL_RCC_TIM4_CLK_DISABLE();
    }
}

} // namespace

Stm32PwmTimer::Stm32PwmTimer(
    TIM_TypeDef* instance,
    uint32_t prescaler,
    uint32_t period)
    : prescaler_(prescaler),
      period_(period) {
    handle_.Instance = instance;
}

bool Stm32PwmTimer::begin() {
    if (initialized_) {
        return true;
    }

    if (!enableTimerClock(handle_.Instance)) {
        return false;
    }

    handle_.Init.Prescaler = prescaler_;
    handle_.Init.CounterMode = TIM_COUNTERMODE_UP;
    handle_.Init.Period = period_;
    handle_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    handle_.Init.RepetitionCounter = 0;
    handle_.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(&handle_) != HAL_OK) {
        HAL_TIM_PWM_DeInit(&handle_);
        disableTimerClock(handle_.Instance);
        return false;
    }

    initialized_ = true;
    return true;
}

bool Stm32PwmTimer::end() {
    if (!initialized_) {
        return true;
    }

    if (HAL_TIM_PWM_DeInit(&handle_) != HAL_OK) {
        return false;
    }

    disableTimerClock(handle_.Instance);

    initialized_ = false;
    return true;
}

TIM_HandleTypeDef* Stm32PwmTimer::handle() {
    return initialized_ ? &handle_ : nullptr;
}

uint32_t Stm32PwmTimer::period() const {
    return period_;
}

} // namespace drivers
