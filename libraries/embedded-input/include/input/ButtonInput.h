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
    using EventCallback = void (*)(
        ButtonEdge edge,
        ButtonGesture gesture,
        void* context
    );

    ButtonInput(
        IButton& button,
        uint32_t debounceMs = 30,
        GestureConfig gestureConfig = {}
    );

    bool begin(uint32_t now) override;
    bool end() override;

    void setCallback(
        EventCallback callback,
        void* context = nullptr
    ) override;

    void update(uint32_t now) override;

    bool isPressed() const override;

private:
    void emit(ButtonEdge edge, ButtonGesture gesture);

    IButton& button_;
    Debouncer debouncer_;
    ButtonGestureRecognizer recognizer_;

    EventCallback callback_ = nullptr;
    void* context_ = nullptr;

    bool initialized_ = false;
};

} // namespace input
