#include "stm32f4xx_hal.h"

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
}
