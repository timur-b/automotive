#include "input/ButtonInput.h"

namespace input {

ButtonInput::ButtonInput(
    IButton& button,
    uint32_t debounceMs,
    GestureConfig gestureConfig)
    : button_(button),
      debouncer_(debounceMs),
      recognizer_(gestureConfig) {}

bool ButtonInput::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!button_.begin()) {
        return false;
    }

    debouncer_.reset(button_.isPressed(), now);
    recognizer_.reset();

    initialized_ = true;
    return true;
}

bool ButtonInput::end() {
    initialized_ = false;

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

    const bool raw = button_.isPressed();

    if (debouncer_.update(raw, now)) {
        const ButtonEdge edge = debouncer_.state()
                                    ? ButtonEdge::PRESSED
                                    : ButtonEdge::RELEASED;

        emit(edge, ButtonGesture::NONE);

        const ButtonGesture gesture =
            recognizer_.onEdge(edge, now);

        if (gesture != ButtonGesture::NONE) {
            emit(ButtonEdge::NONE, gesture);
        }
    }

    const ButtonGesture gesture = recognizer_.update(now);

    if (gesture != ButtonGesture::NONE) {
        emit(ButtonEdge::NONE, gesture);
    }
}

bool ButtonInput::isPressed() const {
    return initialized_ && debouncer_.state();
}

void ButtonInput::emit(
    ButtonEdge edge,
    ButtonGesture gesture) {
    if (callback_ != nullptr) {
        callback_(edge, gesture, context_);
    }
}

} // namespace input
