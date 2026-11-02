#pragma once

#include <cstdint>

#include "stm32f4xx_hal.h"

#include "domain/VehicleState.h"
#include "input/ButtonInput.h"
#include "input/ButtonInputManager.h"
#include "lighting/LightingController.h"
#include "platform/drivers/Stm32Button.h"
#include "platform/drivers/Stm32PwmLed.h"
#include "platform/drivers/Stm32PwmTimer.h"
#include "platform/demo/Stm32DemoController.h"

class ApplicationBootstrap {
public:
    ApplicationBootstrap();

    bool begin(uint32_t now);
    void update(uint32_t now);
    void end();

private:
    drivers::Stm32PwmTimer pwmTimer_;

    drivers::Stm32PwmLed headlights_;
    drivers::Stm32PwmLed brakeLights_;
    drivers::Stm32PwmLed leftTurn_;
    drivers::Stm32PwmLed rightTurn_;

    domain::VehicleState vehicleState_;

    lighting::LightingController lightingController_;

    drivers::Stm32Button button_;
    input::ButtonInput buttonInput_;
    Stm32DemoController demoController_;
    input::ButtonInputManager inputManager_;

    bool initialized_ = false;
};
