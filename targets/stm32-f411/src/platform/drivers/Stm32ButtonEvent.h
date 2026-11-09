#pragma once

#include <cstdint>

namespace drivers {

struct Stm32ButtonEvent {
    uint16_t pin;
    bool pressed;
    uint32_t timestampMs;
};

} // namespace drivers
