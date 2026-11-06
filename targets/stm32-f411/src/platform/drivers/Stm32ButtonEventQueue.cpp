#include "platform/drivers/Stm32ButtonEventQueue.h"

namespace drivers {

bool Stm32ButtonEventQueue::pushFromIsr(
    const Stm32ButtonEvent& event
) {
    const uint32_t head = head_.load(std::memory_order_relaxed);
    const uint32_t next = (head + 1U) % Capacity;

    if (next == tail_.load(std::memory_order_acquire)) {
        droppedEvents_.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    events_[head] = event;

    head_.store(next, std::memory_order_release);
    return true;
}

bool Stm32ButtonEventQueue::pop(Stm32ButtonEvent& event) {
    const uint32_t tail = tail_.load(std::memory_order_relaxed);

    if (tail == head_.load(std::memory_order_acquire)) {
        return false;
    }

    event = events_[tail];

    tail_.store(
        (tail + 1U) % Capacity,
        std::memory_order_release
    );

    return true;
}

uint32_t Stm32ButtonEventQueue::droppedEvents() const {
    return droppedEvents_.load(std::memory_order_relaxed);
}

void Stm32ButtonEventQueue::clear() {
    // Caller must ensure that no producer or consumer is active.
    head_.store(0, std::memory_order_relaxed);
    tail_.store(0, std::memory_order_relaxed);
}

void Stm32ButtonEventQueue::reset() {
    clear();
    droppedEvents_.store(0, std::memory_order_relaxed);
}

} // namespace drivers
