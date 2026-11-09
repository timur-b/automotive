#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "platform/drivers/Stm32ButtonEventQueue.h"
#include "platform/drivers/Stm32ButtonInterruptManager.h"
#include "platform/drivers/Stm32ButtonInterruptWorker.h"
#include "platform/drivers/Stm32PeriodicTimer.h"
#include "platform/Board.h"

namespace drivers {

class Stm32ButtonInterruptController {
public:
    enum class PopResult {
        Event,
        Empty,
        Overflow
    };

    static constexpr std::size_t ButtonCount = 6;

    struct ButtonStateSnapshot {
        std::array<bool, ButtonCount> pressed{};
        uint32_t droppedEvents = 0;
    };

    Stm32ButtonInterruptController();

    bool begin(uint32_t now);
    void end();

    void onExtiInterrupt(uint16_t pin);
    void onTimerInterrupt(uint32_t now);
    void handleTimerIrq();

    bool popEvent(Stm32ButtonEvent& event);

    PopResult popEventChecked(
        Stm32ButtonEvent& event,
        uint32_t lastObservedDroppedEvents
    );

    bool getStableState(uint16_t pin, bool& pressed) const;

    uint32_t droppedEvents() const;

    bool recoverAfterOverflow(ButtonStateSnapshot& snapshot);

private:
    Stm32ButtonEventQueue queue_;
    std::array<Stm32ButtonInterruptWorker, ButtonCount> workers_;
    Stm32ButtonInterruptManager manager_;
    Stm32PeriodicTimer timer_;

    bool workersRegistered_ = false;
    bool initialized_ = false;
};

} // namespace drivers
