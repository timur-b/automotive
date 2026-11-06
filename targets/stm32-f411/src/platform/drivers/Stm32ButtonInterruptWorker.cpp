#include "platform/drivers/Stm32ButtonInterruptWorker.h"

namespace drivers {

Stm32ButtonInterruptWorker::Stm32ButtonInterruptWorker(
    GPIO_TypeDef* port,
    uint16_t pin,
    Stm32ButtonEventQueue& queue,
    uint32_t debounceMs
)
    : port_(port),
      pin_(pin),
      queue_(queue),
      debouncer_(debounceMs) {}

void Stm32ButtonInterruptWorker::begin(uint32_t now) {
    const bool pressed =
        HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET;

    debouncer_.reset(pressed, now);
    stablePressed_.store(pressed, std::memory_order_relaxed);

    rawEdges_ = 0;
    stableEdges_ = 0;
    droppedEvents_ = 0;
    tracking_ = false;
    edgeGeneration_ = 0;
    lastObservedGeneration_ = 0;
    lastActivityAt_ = now;
}

void Stm32ButtonInterruptWorker::onExtiInterrupt(uint16_t pin) {
    if (pin != pin_) {
        return;
    }

    ++rawEdges_;
    ++edgeGeneration_;
    tracking_ = true;
}

void Stm32ButtonInterruptWorker::onTimerInterrupt(uint32_t now) {
    if (!tracking_) {
        return;
    }

    const uint32_t generation = edgeGeneration_;

    if (generation != lastObservedGeneration_) {
        lastObservedGeneration_ = generation;
        lastActivityAt_ = now;
    }

    const bool pressed =
        HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET;

    if (debouncer_.update(pressed, now)) {
        ++stableEdges_;

        const bool stablePressed = debouncer_.state();
        stablePressed_.store(stablePressed, std::memory_order_release);

        const Stm32ButtonEvent event{
            pin_,
            stablePressed,
            now
        };

        if (!queue_.pushFromIsr(event)) {
            ++droppedEvents_;
        }
    }

    if (static_cast<uint32_t>(now - lastActivityAt_) >= QuietPeriodMs &&
        pressed == debouncer_.state() &&
        generation == edgeGeneration_) {
        tracking_ = false;
    }
}

uint32_t Stm32ButtonInterruptWorker::rawEdges() const {
    return rawEdges_;
}

uint32_t Stm32ButtonInterruptWorker::stableEdges() const {
    return stableEdges_;
}

uint16_t Stm32ButtonInterruptWorker::pin() const {
    return pin_;
}

bool Stm32ButtonInterruptWorker::isPressed() const {
    return stablePressed_.load(std::memory_order_acquire);
}

uint32_t Stm32ButtonInterruptWorker::droppedEvents() const {
    return droppedEvents_;
}

} // namespace drivers
