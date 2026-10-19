#include "input/ButtonGestureRecognizer.h"

namespace input {

ButtonGestureRecognizer::ButtonGestureRecognizer(GestureConfig config) : config_(config) {}

bool ButtonGestureRecognizer::elapsed(uint32_t now, uint32_t since, uint32_t duration) {
    return static_cast<uint32_t>(now - since) >= duration;
}

void ButtonGestureRecognizer::reset() {
    state_ = State::IDLE;
    pressedAt_ = 0;
    releasedAt_ = 0;
}

ButtonGesture ButtonGestureRecognizer::onEdge(ButtonEdge edge, uint32_t now) {
    if (edge == ButtonEdge::NONE) {
        return ButtonGesture::NONE;
    }

    switch (state_) {

        case State::IDLE:
            if (edge == ButtonEdge::PRESSED) {
                pressedAt_ = now;
                state_ = State::FIRST_DOWN;
            }
            break;

        case State::FIRST_DOWN:
            if (edge == ButtonEdge::RELEASED) {

                // Detect a long press even if update()
                // was not called before the button was released.
                if (elapsed(now, pressedAt_, config_.longPressMs)) {
                    state_ = State::IDLE;
                    return ButtonGesture::LONG;
                }

                releasedAt_ = now;
                state_ = State::WAIT_SECOND;
            }
            break;

        case State::WAIT_SECOND:
            if (edge == ButtonEdge::PRESSED) {

                // Check whether the double-click window
                // has already expired.
                if (elapsed(now, releasedAt_, config_.doubleClickMs)) {

                    // Complete the first click as SINGLE.
                    // Treat the current press as the beginning
                    // of a new gesture sequence.
                    pressedAt_ = now;
                    state_ = State::FIRST_DOWN;

                    return ButtonGesture::SINGLE;
                }

                pressedAt_ = now;
                state_ = State::SECOND_DOWN;
            }
            break;

        case State::SECOND_DOWN:
            if (edge == ButtonEdge::RELEASED) {

                // Detect a long press even if the periodic
                // update() call was delayed.
                if (elapsed(now, pressedAt_, config_.longPressMs)) {
                    state_ = State::IDLE;
                    return ButtonGesture::LONG;
                }

                state_ = State::IDLE;
                return ButtonGesture::DOUBLE;
            }
            break;

        case State::LONG_HELD:
            if (edge == ButtonEdge::RELEASED) {
                state_ = State::IDLE;
            }
            break;
    }

    return ButtonGesture::NONE;
}

ButtonGesture ButtonGestureRecognizer::update(uint32_t now) {
    switch (state_) {

        case State::FIRST_DOWN:
        case State::SECOND_DOWN:
            // Generate LONG once the button has been held
            // for the configured duration.
            if (elapsed(now, pressedAt_, config_.longPressMs)) {
                state_ = State::LONG_HELD;
                return ButtonGesture::LONG;
            }
            break;

        case State::WAIT_SECOND:
            // Generate SINGLE if no second press occurs
            // within the double-click interval.
            if (elapsed(now, releasedAt_, config_.doubleClickMs)) {
                state_ = State::IDLE;
                return ButtonGesture::SINGLE;
            }
            break;

        default:
            break;
    }

    return ButtonGesture::NONE;
}

} // namespace input
