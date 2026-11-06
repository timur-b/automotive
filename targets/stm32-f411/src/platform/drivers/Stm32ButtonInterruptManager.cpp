#include "platform/drivers/Stm32ButtonInterruptManager.h"

namespace drivers {

bool Stm32ButtonInterruptManager::add(
    Stm32ButtonInterruptWorker& worker
) {
    if (count_ >= MaxButtons) {
        return false;
    }

    workers_[count_++] = &worker;
    return true;
}

void Stm32ButtonInterruptManager::begin(uint32_t now) {
    for (size_t i = 0; i < count_; ++i) {
        workers_[i]->begin(now);
    }
}

void Stm32ButtonInterruptManager::onExtiInterrupt(uint16_t pin) {
    for (size_t i = 0; i < count_; ++i) {
        workers_[i]->onExtiInterrupt(pin);
    }
}

void Stm32ButtonInterruptManager::onTimerInterrupt(uint32_t now) {
    for (size_t i = 0; i < count_; ++i) {
        workers_[i]->onTimerInterrupt(now);
    }
}

} // namespace drivers
