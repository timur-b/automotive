#include "platform/drivers/Stm32PeriodicTimer.h"

namespace drivers {

bool Stm32PeriodicTimer::begin() {
    if (initialized_) {
        return true;
    }

    __HAL_RCC_TIM3_CLK_ENABLE();

    // Initial values for a 16 MHz TIM3 input clock:
    // 16 MHz / 16000 = 1000 Hz
    // 1000 Hz / 5 = 200 Hz (5 ms)
    handle_.Instance = TIM3;
    handle_.Init.Prescaler = 15999;
    handle_.Init.Period = 4;
    handle_.Init.CounterMode = TIM_COUNTERMODE_UP;
    handle_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&handle_) != HAL_OK) {
        __HAL_RCC_TIM3_CLK_DISABLE();
        return false;
    }

    ticks_ = 0;

    HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    if (HAL_TIM_Base_Start_IT(&handle_) != HAL_OK) {
        HAL_NVIC_DisableIRQ(TIM3_IRQn);
        HAL_TIM_Base_DeInit(&handle_);
        __HAL_RCC_TIM3_CLK_DISABLE();
        return false;
    }

    initialized_ = true;
    return true;
}

void Stm32PeriodicTimer::end() {
    if (!initialized_) {
        return;
    }

    HAL_TIM_Base_Stop_IT(&handle_);
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
    HAL_TIM_Base_DeInit(&handle_);
    __HAL_RCC_TIM3_CLK_DISABLE();

    initialized_ = false;
}

uint32_t Stm32PeriodicTimer::ticks() const {
    return ticks_;
}

void Stm32PeriodicTimer::handleIrq() {
    HAL_TIM_IRQHandler(&handle_);
}

void Stm32PeriodicTimer::onInterrupt() {
    ++ticks_;
}

} // namespace drivers
