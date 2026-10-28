#pragma once

#include <cstdint>

#include "input/ButtonEvents.h"
#include "input/ButtonGestureRecognizer.h"
#include "input/Debouncer.h"
#include "input/IButton.h"
#include "input/IButtonInput.h"

namespace input {

class ButtonInput : public IButtonInput {
public:
    using EventCallback = void (*)(ButtonEdge edge, ButtonGesture gesture, void* context);

    ButtonInput(IButton& button, uint32_t debounceMs = 30, GestureConfig gestureConfig = {})
        : button_(button), debouncer_(debounceMs), recognizer_(gestureConfig) {}

    bool begin(uint32_t now) override {
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

    bool end() override {
        initialized_ = false;

        recognizer_.reset();
        debouncer_.reset(false, 0);

        return button_.end();
    }

    void setCallback(EventCallback callback, void* context = nullptr) {
        callback_ = callback;
        context_ = context;
    }

    void update(uint32_t now) {
        if (!initialized_) {
            return;
        }

        const bool raw = button_.isPressed();

        if (debouncer_.update(raw, now)) {
            const ButtonEdge edge = debouncer_.state() ? ButtonEdge::PRESSED : ButtonEdge::RELEASED;

            emit(edge, ButtonGesture::NONE);

            const ButtonGesture gesture = recognizer_.onEdge(edge, now);

            if (gesture != ButtonGesture::NONE) {
                emit(ButtonEdge::NONE, gesture);
            }
        }

        const ButtonGesture gesture = recognizer_.update(now);

        if (gesture != ButtonGesture::NONE) {
            emit(ButtonEdge::NONE, gesture);
        }
    }

    bool isPressed() const {
        return initialized_ && debouncer_.state();
    }

private:
    void emit(ButtonEdge edge, ButtonGesture gesture) {
        if (callback_ != nullptr) {
            callback_(edge, gesture, context_);
        }
    }

    IButton& button_;
    Debouncer debouncer_;
    ButtonGestureRecognizer recognizer_;

    EventCallback callback_ = nullptr;
    void* context_ = nullptr;

    bool initialized_ = false;
};

} // namespace input
