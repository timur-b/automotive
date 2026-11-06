#include "platform/drivers/Stm32ButtonInterruptController.h"

namespace drivers {

Stm32ButtonInterruptController::Stm32ButtonInterruptController()
    : workers_{{
          {Board::ENGINE_BUTTON.port, Board::ENGINE_BUTTON.pin, queue_, 50},
          {Board::LIGHT_BUTTON.port, Board::LIGHT_BUTTON.pin, queue_, 50},
          {Board::TURN_BUTTON.port, Board::TURN_BUTTON.pin, queue_, 50},
          {Board::BRAKE_BUTTON.port, Board::BRAKE_BUTTON.pin, queue_, 50},
          {Board::CRASH_BUTTON.port, Board::CRASH_BUTTON.pin, queue_, 50},
          {Board::LOCK_BUTTON.port, Board::LOCK_BUTTON.pin, queue_, 50}
      }} {
}

bool Stm32ButtonInterruptController::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!workersRegistered_) {
        for (auto& worker : workers_) {
            if (!manager_.add(worker)) {
                return false;
            }
        }

        workersRegistered_ = true;
    }

    // No producer or consumer may access the queue here.
    queue_.reset();

    // Reset debounce and edge-tracking state.
    manager_.begin(now);

    // Clear pending EXTI requests.
    __HAL_GPIO_EXTI_CLEAR_IT(
        GPIO_PIN_1 |
        GPIO_PIN_2 |
        GPIO_PIN_3 |
        GPIO_PIN_4 |
        GPIO_PIN_5 |
        GPIO_PIN_6
    );

    // Mark the controller ready before starting TIM3 interrupts.
    initialized_ = true;

    if (!timer_.begin()) {
        initialized_ = false;
        return false;
    }

    const IRQn_Type irqs[] = {
        EXTI1_IRQn,
        EXTI2_IRQn,
        EXTI3_IRQn,
        EXTI4_IRQn,
        EXTI9_5_IRQn
    };

    for (IRQn_Type irq : irqs) {
        HAL_NVIC_SetPriority(irq, 6, 0);
        HAL_NVIC_ClearPendingIRQ(irq);
        HAL_NVIC_EnableIRQ(irq);
    }

    return true;
}

void Stm32ButtonInterruptController::end() {
    if (!initialized_) {
        return;
    }

    const IRQn_Type irqs[] = {
        EXTI1_IRQn,
        EXTI2_IRQn,
        EXTI3_IRQn,
        EXTI4_IRQn,
        EXTI9_5_IRQn
    };

    // Stop accepting new GPIO interrupts.
    for (IRQn_Type irq : irqs) {
        HAL_NVIC_DisableIRQ(irq);
    }

    // Stop periodic debounce processing.
    timer_.end();

    // Clear pending EXTI requests.
    __HAL_GPIO_EXTI_CLEAR_IT(
        GPIO_PIN_1 |
        GPIO_PIN_2 |
        GPIO_PIN_3 |
        GPIO_PIN_4 |
        GPIO_PIN_5 |
        GPIO_PIN_6
    );

    initialized_ = false;
}

void Stm32ButtonInterruptController::onExtiInterrupt(uint16_t pin) {
    if (initialized_) {
        manager_.onExtiInterrupt(pin);
    }
}

void Stm32ButtonInterruptController::onTimerInterrupt(uint32_t now) {
    if (!initialized_) {
        return;
    }

    timer_.onInterrupt();
    manager_.onTimerInterrupt(now);
}

void Stm32ButtonInterruptController::handleTimerIrq() {
    timer_.handleIrq();
}

bool Stm32ButtonInterruptController::popEvent(
    Stm32ButtonEvent& event
) {
    return queue_.pop(event);
}

Stm32ButtonInterruptController::PopResult
Stm32ButtonInterruptController::popEventChecked(
    Stm32ButtonEvent& event,
    uint32_t lastObservedDroppedEvents
) {
    const uint32_t primask = __get_PRIMASK();
    __disable_irq();

    PopResult result;

    if (queue_.droppedEvents() != lastObservedDroppedEvents) {
        result = PopResult::Overflow;
    } else if (queue_.pop(event)) {
        result = PopResult::Event;
    } else {
        result = PopResult::Empty;
    }

    __set_PRIMASK(primask);
    return result;
}

bool Stm32ButtonInterruptController::getStableState(
    uint16_t pin,
    bool& pressed
) const {
    for (const auto& worker : workers_) {
        if (worker.pin() == pin) {
            pressed = worker.isPressed();
            return true;
        }
    }

    return false;
}

uint32_t Stm32ButtonInterruptController::droppedEvents() const {
    return queue_.droppedEvents();
}

bool Stm32ButtonInterruptController::recoverAfterOverflow(
    ButtonStateSnapshot& snapshot
) {
    if (!initialized_) {
        return false;
    }

    // Save the current interrupt masking state.
    const uint32_t primask = __get_PRIMASK();

    // Prevent interrupt handlers from changing worker states
    // or producing new queue events during recovery.
    __disable_irq();

    for (std::size_t i = 0; i < workers_.size(); ++i) {
        snapshot.pressed[i] = workers_[i].isPressed();
    }

    // Discard stale events but preserve overflow statistics.
    queue_.clear();

    snapshot.droppedEvents = queue_.droppedEvents();

    // Restore the previous interrupt masking state.
    __set_PRIMASK(primask);

    return true;
}

} // namespace drivers
