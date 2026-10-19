#pragma once

#include "input/ButtonEvents.h"
#include <cstdint>

namespace input {

struct GestureConfig {
    uint32_t longPressMs = 800;
    uint32_t doubleClickMs = 300;
};

class ButtonGestureRecognizer {
public:
    explicit ButtonGestureRecognizer(GestureConfig config = {});

    void reset();

    ButtonGesture onEdge(ButtonEdge edge, uint32_t now);
    ButtonGesture update(uint32_t now);

private:
    enum class State { IDLE, FIRST_DOWN, WAIT_SECOND, SECOND_DOWN, LONG_HELD };

    static bool elapsed(uint32_t now, uint32_t since, uint32_t duration);

    State state_ = State::IDLE;
    GestureConfig config_;

    uint32_t pressedAt_ = 0;
    uint32_t releasedAt_ = 0;
};

} // namespace input
