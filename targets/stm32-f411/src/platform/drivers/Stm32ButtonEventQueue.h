#pragma once

#include <atomic>
#include <cstdint>

#include "platform/drivers/Stm32ButtonEvent.h"

namespace drivers {

class Stm32ButtonEventQueue {
public:
    static constexpr uint32_t Capacity = 16;

    bool pushFromIsr(const Stm32ButtonEvent& event);
    bool pop(Stm32ButtonEvent& event);

    uint32_t droppedEvents() const;

    void clear();
    void reset();

private:
    static_assert(
        std::atomic<uint32_t>::is_always_lock_free,
        "Queue indices must be lock-free"
    );

    Stm32ButtonEvent events_[Capacity] = {};

    std::atomic<uint32_t> head_{0};
    std::atomic<uint32_t> tail_{0};
    std::atomic<uint32_t> droppedEvents_{0};
};

} // namespace drivers
