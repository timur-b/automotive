#pragma once

#include "domain/VehicleState.h"
#include "input/IButtonInput.h"

class Stm32DemoController {
public:
    Stm32DemoController(
        input::IButtonInput& button,
        domain::VehicleState& state
    );

    void begin();
    void end();

private:
    static void buttonCallback(
        input::ButtonEdge edge,
        input::ButtonGesture gesture,
        void* context
    );

    void handleGesture(input::ButtonGesture gesture);

    input::IButtonInput& button_;
    domain::VehicleState& state_;
};
