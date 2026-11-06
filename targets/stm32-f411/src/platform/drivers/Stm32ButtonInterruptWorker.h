#pragma once

#include <cstdint>
#include <atomic>

#include "stm32f4xx_hal.h"
#include "input/Debouncer.h"
#include "platform/drivers/Stm32ButtonEventQueue.h"

namespace drivers {

class Stm32ButtonInterruptWorker {
public:
    Stm32ButtonInterruptWorker(
        GPIO_TypeDef* port,
        uint16_t pin,
        Stm32ButtonEventQueue& queue,
        uint32_t debounceMs = 50
    );

    void begin(uint32_t now);

    void onExtiInterrupt(uint16_t pin);
    void onTimerInterrupt(uint32_t now);

    uint32_t rawEdges() const;
    uint32_t stableEdges() const;

    uint16_t pin() const;
    bool isPressed() const;

    uint32_t droppedEvents() const;

private:
    static_assert(
        std::atomic<bool>::is_always_lock_free,
        "Button state must be lock-free"
    );

    std::atomic<bool> stablePressed_{false};

    GPIO_TypeDef* port_;
    uint16_t pin_;

    Stm32ButtonEventQueue& queue_;
    input::Debouncer debouncer_;

    volatile uint32_t rawEdges_ = 0;
    volatile uint32_t stableEdges_ = 0;
    volatile bool tracking_ = false;
    volatile uint32_t edgeGeneration_ = 0;
    uint32_t lastObservedGeneration_ = 0;
    uint32_t lastActivityAt_ = 0;
    static constexpr uint32_t QuietPeriodMs = 50;
    volatile uint32_t droppedEvents_ = 0;
};

} // namespace drivers
