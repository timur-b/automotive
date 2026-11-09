#pragma once

#include <cstddef>
#include <cstdint>

#include "platform/drivers/Stm32ButtonInterruptWorker.h"

namespace drivers {

class Stm32ButtonInterruptManager {
public:
    static constexpr size_t MaxButtons = 6;

    bool add(Stm32ButtonInterruptWorker& worker);
    void begin(uint32_t now);

    void onExtiInterrupt(uint16_t pin);
    void onTimerInterrupt(uint32_t now);

private:
    Stm32ButtonInterruptWorker* workers_[MaxButtons] = {};
    size_t count_ = 0;
};

} // namespace drivers
