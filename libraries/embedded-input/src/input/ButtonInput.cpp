#include "input/ButtonInput.h"

namespace input {

ButtonInput::ButtonInput(
    IButton& button,
    uint32_t debounceMs,
    GestureConfig gestureConfig,
    InputMode mode)
    : button_(button),
      debouncer_(debounceMs),
      recognizer_(gestureConfig),
      mode_(mode) {}

bool ButtonInput::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!button_.begin()) {
        return false;
    }

    stablePressed_ = button_.isPressed();

    debouncer_.reset(stablePressed_, now);
    recognizer_.reset();

    initialized_ = true;
    return true;
}

bool ButtonInput::end() {
    initialized_ = false;

    stablePressed_ = false;
    recognizer_.reset();
    debouncer_.reset(false, 0);

    return button_.end();
}

void ButtonInput::setCallback(
    EventCallback callback,
    void* context) {
    callback_ = callback;
    context_ = context;
}

void ButtonInput::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    if (mode_ == InputMode::Polling) {
        const bool raw = button_.isPressed();

        if (debouncer_.update(raw, now)) {
            stablePressed_ = debouncer_.state();

            processEdge(
                stablePressed_
                    ? ButtonEdge::PRESSED
                    : ButtonEdge::RELEASED,
                now
            );
        }
    }

    const ButtonGesture gesture = recognizer_.update(now);

    if (gesture != ButtonGesture::NONE) {
        emit(ButtonEdge::NONE, gesture);
    }
}

bool ButtonInput::isPressed() const {
    return initialized_ && stablePressed_;
}

void ButtonInput::emit(
    ButtonEdge edge,
    ButtonGesture gesture) {
    if (callback_ != nullptr) {
        callback_(edge, gesture, context_);
    }
}

void ButtonInput::processEdge(ButtonEdge edge, uint32_t now) {
    emit(edge, ButtonGesture::NONE);

    const ButtonGesture gesture = recognizer_.onEdge(edge, now);

    if (gesture != ButtonGesture::NONE) {
        emit(ButtonEdge::NONE, gesture);
    }
}

void ButtonInput::onStableStateChanged(
    bool pressed,
    uint32_t timestampMs
) {
    if (!initialized_ || mode_ != InputMode::ExternalEvents) {
        return;
    }

    if (stablePressed_ == pressed) {
        return;
    }

    stablePressed_ = pressed;

    processEdge(
        pressed ? ButtonEdge::PRESSED : ButtonEdge::RELEASED,
        timestampMs
    );
}

void ButtonInput::resyncStableState(bool pressed, uint32_t now) {
    if (!initialized_ || mode_ != InputMode::ExternalEvents) {
        return;
    }

    stablePressed_ = pressed;
    debouncer_.reset(pressed, now);
    recognizer_.reset();
}

} // namespace input
