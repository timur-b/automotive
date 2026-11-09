#include "stm32f4xx_hal.h"
#include "platform/bootstrap/ApplicationBootstrap.h"

static ApplicationBootstrap* application = nullptr;

void registerInterruptApplication(ApplicationBootstrap* app) {
    application = app;
}

extern "C" void EXTI1_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

extern "C" void EXTI2_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

extern "C" void EXTI3_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

extern "C" void EXTI4_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

extern "C" void EXTI9_5_IRQHandler() {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t pin) {
    if (application != nullptr) {
        application->onButtonExtiInterrupt(pin);
    }
}

extern "C" void TIM3_IRQHandler() {
    if (application != nullptr) {
        application->handleButtonTimerIrq();
    }
}

extern "C" void HAL_TIM_PeriodElapsedCallback(
    TIM_HandleTypeDef* htim
) {
    if (htim->Instance == TIM3 && application != nullptr) {
        application->onButtonTimerInterrupt(HAL_GetTick());
    }
}
