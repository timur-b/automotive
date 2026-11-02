#include "Stm32DemoController.h"

Stm32DemoController::Stm32DemoController(
    input::IButtonInput& button,
    domain::VehicleState& state
)
    : button_(button),
      state_(state) {
}

void Stm32DemoController::begin() {
    button_.setCallback(buttonCallback, this);
}

void Stm32DemoController::end() {
    button_.setCallback(nullptr);
}

void Stm32DemoController::buttonCallback(
    input::ButtonEdge edge,
    input::ButtonGesture gesture,
    void* context
) {
    (void)edge;

    auto* controller =
        static_cast<Stm32DemoController*>(context);

    if (controller != nullptr) {
        controller->handleGesture(gesture);
    }
}

void Stm32DemoController::handleGesture(
    input::ButtonGesture gesture
) {
    switch (gesture) {
        case input::ButtonGesture::SINGLE:
            switch (state_.turnSignal) {
                case domain::TurnSignal::OFF:
                    state_.turnSignal = domain::TurnSignal::LEFT;
                    break;

                case domain::TurnSignal::LEFT:
                    state_.turnSignal = domain::TurnSignal::RIGHT;
                    break;

                case domain::TurnSignal::RIGHT:
                    state_.turnSignal = domain::TurnSignal::HAZARD;
                    break;

                case domain::TurnSignal::HAZARD:
                    state_.turnSignal = domain::TurnSignal::OFF;
                    break;
            }
            break;

        case input::ButtonGesture::DOUBLE:
            state_.headlightMode =
                state_.headlightMode == domain::HeadlightMode::ON
                    ? domain::HeadlightMode::OFF
                    : domain::HeadlightMode::ON;
            break;

        case input::ButtonGesture::LONG:
            state_.crashDetected = !state_.crashDetected;
            break;

        default:
            break;
    }
}
