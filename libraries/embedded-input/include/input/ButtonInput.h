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
    enum class InputMode {
        Polling,
        ExternalEvents
    };

    using EventCallback = void (*)(
        ButtonEdge edge,
        ButtonGesture gesture,
        void* context
    );

    ButtonInput(
        IButton& button,
        uint32_t debounceMs = 30,
        GestureConfig gestureConfig = {},
        InputMode mode = InputMode::Polling
    );

    bool begin(uint32_t now) override;
    bool end() override;

    void setCallback(
        EventCallback callback,
        void* context = nullptr
    ) override;

    void update(uint32_t now) override;

    bool isPressed() const override;

    void onStableStateChanged(bool pressed, uint32_t timestampMs);

    void resyncStableState(bool pressed, uint32_t now);

private:
    void emit(ButtonEdge edge, ButtonGesture gesture);

    void processEdge(ButtonEdge edge, uint32_t now);

    IButton& button_;
    Debouncer debouncer_;
    ButtonGestureRecognizer recognizer_;
    InputMode mode_;

    EventCallback callback_ = nullptr;
    void* context_ = nullptr;
    bool stablePressed_ = false;

    bool initialized_ = false;
};

} // namespace input
