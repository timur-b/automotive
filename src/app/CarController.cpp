#include "app/CarController.h"

#include "config/CarConfig.h"

#include <algorithm>

namespace app {

CarController::CarController(input::IButtonInput& lightButton, input::IButtonInput& brakeButton,
                             input::IButtonInput& turnButton, input::IButtonInput& crashButton,
                             input::IButtonInput& engineButton, IEncoderInput& encoder)
    : lightButton_(lightButton), brakeButton_(brakeButton), turnButton_(turnButton),
      crashButton_(crashButton), engineButton_(engineButton), encoder_(encoder), lightContext_{this, CarButton::LIGHT},
      brakeContext_{this, CarButton::BRAKE}, turnContext_{this, CarButton::TURN},
      crashContext_{this, CarButton::CRASH}, engineContext_{this, CarButton::ENGINE} {}

void CarController::begin() {
    // Register callbacks for each input.
    lightButton_.setCallback(&CarController::buttonCallback, &lightContext_);

    brakeButton_.setCallback(&CarController::buttonCallback, &brakeContext_);

    turnButton_.setCallback(&CarController::buttonCallback, &turnContext_);

    crashButton_.setCallback(&CarController::buttonCallback, &crashContext_);

    engineButton_.setCallback(&CarController::buttonCallback, &engineContext_);

    // Synchronize the initial brake state.
    state_.brakePressed = brakeButton_.isPressed();
}

void CarController::update(uint32_t now) {
    const int32_t encoderDelta = encoder_.consumeDelta();

    if (encoderDelta != 0) {
        handleEncoderDelta(encoderDelta);
    }

    if (!state_.brakePressed) {
        brakeTimingActive_ = false;
        state_.emergencyBrakePhase = domain::EmergencyBrakePhase::OFF;
        return;
    }

    if (!brakeTimingActive_) {
        brakePressedAt_ = now;
        brakeTimingActive_ = true;
    }

    const uint32_t elapsed = static_cast<uint32_t>(now - brakePressedAt_);

    if (elapsed < config::EMERGENCY_BRAKE_HOLD_MS) {
        state_.emergencyBrakePhase = domain::EmergencyBrakePhase::OFF;
    } else if (elapsed <
               config::EMERGENCY_BRAKE_HOLD_MS + config::EMERGENCY_BRAKE_RAPID_DURATION_MS) {

        state_.emergencyBrakePhase = domain::EmergencyBrakePhase::RAPID_FLASHING;
    } else {
        state_.emergencyBrakePhase = domain::EmergencyBrakePhase::HAZARD_FLASHING;
    }
}

void CarController::handleEncoderDelta(int32_t delta) {
    if (state_.engineState != domain::EngineState::RUNNING || delta == 0) {
        return;
    }
    
    constexpr int32_t MIN_POWER = 50;
    constexpr int32_t MAX_POWER = 100;
    constexpr int32_t POWER_STEP = 5;

    const int64_t requested =
        static_cast<int64_t>(state_.throttlePercent) +
        static_cast<int64_t>(delta) * POWER_STEP;

    const int64_t clamped =
        std::clamp(
            requested,
            static_cast<int64_t>(MIN_POWER),
            static_cast<int64_t>(MAX_POWER)
        );

    state_.throttlePercent = static_cast<uint8_t>(clamped);
}

void CarController::setAmbientDark(bool dark) {
    state_.ambientDark = dark;
}

const domain::VehicleState& CarController::state() const {
    return state_;
}

void CarController::buttonCallback(input::ButtonEdge edge, input::ButtonGesture gesture,
                                   void* context) {
    auto* buttonContext = static_cast<ButtonContext*>(context);

    if (buttonContext == nullptr || buttonContext->controller == nullptr) {
        return;
    }

    buttonContext->controller->onButtonEvent(buttonContext->button, edge, gesture);
}

void CarController::onButtonEvent(CarButton button, input::ButtonEdge edge,
                                  input::ButtonGesture gesture) {
    switch (button) {

        case CarButton::LIGHT:
            if (gesture == input::ButtonGesture::SINGLE) {
                switch (state_.headlightMode) {
                    case domain::HeadlightMode::OFF:
                        state_.headlightMode = domain::HeadlightMode::ON;
                        break;

                    case domain::HeadlightMode::ON:
                        state_.headlightMode = domain::HeadlightMode::AUTO;
                        break;

                    case domain::HeadlightMode::AUTO:
                        state_.headlightMode = domain::HeadlightMode::OFF;
                        break;
                }
            }
            break;

        case CarButton::BRAKE:
            if (edge == input::ButtonEdge::PRESSED) {
                state_.brakePressed = true;
                brakeTimingActive_ = false;
                state_.emergencyBrakePhase = domain::EmergencyBrakePhase::OFF;
            }

            if (edge == input::ButtonEdge::RELEASED) {
                state_.brakePressed = false;
                brakeTimingActive_ = false;
                state_.emergencyBrakePhase = domain::EmergencyBrakePhase::OFF;
            }
            break;

        case CarButton::TURN:
            // Single: toggle left turn signal.
            if (gesture == input::ButtonGesture::SINGLE) {
                state_.turnSignal = state_.turnSignal == domain::TurnSignal::LEFT
                                        ? domain::TurnSignal::OFF
                                        : domain::TurnSignal::LEFT;
            }

            // Double: toggle right turn signal.
            if (gesture == input::ButtonGesture::DOUBLE) {
                state_.turnSignal = state_.turnSignal == domain::TurnSignal::RIGHT
                                        ? domain::TurnSignal::OFF
                                        : domain::TurnSignal::RIGHT;
            }

            // Long: toggle hazard lights.
            if (gesture == input::ButtonGesture::LONG) {
                state_.turnSignal = state_.turnSignal == domain::TurnSignal::HAZARD
                                        ? domain::TurnSignal::OFF
                                        : domain::TurnSignal::HAZARD;
            }
            break;

        case CarButton::CRASH:
            if (gesture == input::ButtonGesture::SINGLE) {
                state_.crashDetected = true;
                state_.airbagsDeployed = true;
                state_.engineState = domain::EngineState::STOPPED_BY_CRASH;
            }

            if (gesture == input::ButtonGesture::LONG) {
                state_.crashDetected = false;
                state_.airbagsDeployed = false;
                state_.engineState = domain::EngineState::OFF;
            }
            break;

        case CarButton::ENGINE:
            if (gesture == input::ButtonGesture::SINGLE) {
                switch (state_.engineState) {
                    case domain::EngineState::OFF:
                        state_.throttlePercent = 50;
                        state_.engineState = domain::EngineState::RUNNING;
                        break;

                    case domain::EngineState::RUNNING:
                        state_.engineState = domain::EngineState::OFF;
                        break;

                    case domain::EngineState::STOPPED_BY_CRASH:
                        // Engine restart is blocked until crash reset.
                        break;
                }
            }
            break;
    }
}

} // namespace app
