#pragma once

#include "stm32f4xx_hal.h"

#include <cstdint>

namespace drivers {

class Stm32PwmTimer final {
public:
    Stm32PwmTimer(TIM_TypeDef* instance,
                  uint32_t prescaler,
                  uint32_t period);

    bool begin();
    bool end();

    TIM_HandleTypeDef* handle();
    uint32_t period() const;

private:
    TIM_HandleTypeDef handle_{};

    uint32_t prescaler_;
    uint32_t period_;

    bool initialized_ = false;
};

} // namespace drivers
