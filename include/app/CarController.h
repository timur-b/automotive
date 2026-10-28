#pragma once

#include <cstdint>

#include "domain/VehicleState.h"
#include "input/IButtonInput.h"
#include "input/IEncoderInput.h"

namespace app {

enum class CarButton : uint8_t { LIGHT, BRAKE, TURN, CRASH, ENGINE };

class CarController {
public:
    CarController(input::IButtonInput& lightButton, input::IButtonInput& brakeButton,
                  input::IButtonInput& turnButton, input::IButtonInput& crashButton,
                  input::IButtonInput& engineButton, IEncoderInput& encoder);

    void begin();
    void end();
    void update(uint32_t now);

    void setAmbientDark(bool dark);

    const domain::VehicleState& state() const;

private:
    struct ButtonContext {
        CarController* controller;
        CarButton button;
    };

    static void buttonCallback(input::ButtonEdge edge, input::ButtonGesture gesture, void* context);

    void onButtonEvent(CarButton button, input::ButtonEdge edge, input::ButtonGesture gesture);

    input::IButtonInput& lightButton_;
    input::IButtonInput& brakeButton_;
    input::IButtonInput& turnButton_;
    input::IButtonInput& crashButton_;
    input::IButtonInput& engineButton_;

    ButtonContext lightContext_;
    ButtonContext brakeContext_;
    ButtonContext turnContext_;
    ButtonContext crashContext_;
    ButtonContext engineContext_;

    domain::VehicleState state_;

    uint32_t brakePressedAt_ = 0;
    uint32_t emergencyBrakingStartedAt_ = 0;

    bool brakeTimingActive_ = false;

    IEncoderInput& encoder_;

    void handleEncoderDelta(int32_t delta);
};
} // namespace app
