#pragma once

#include <cstdint>
#include "stm32f4xx_hal.h"

namespace drivers {

class Stm32PeriodicTimer {
public:
    bool begin();
    void end();

    uint32_t ticks() const;

    void handleIrq();
    void onInterrupt();

private:
    TIM_HandleTypeDef handle_{};
    volatile uint32_t ticks_ = 0;
    bool initialized_ = false;
};

} // namespace drivers
