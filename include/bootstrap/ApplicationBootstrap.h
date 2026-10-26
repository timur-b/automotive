#pragma once

#include <cstdint>

#include "app/AmbientLightController.h"
#include "app/CarController.h"
#include "drivers/AdafruitNeoPixelLed.h"
#include "drivers/Button.h"
#include "drivers/Esp32PwmLed.h"
#include "drivers/Ky040Encoder.h"
#include "drivers/Photoresistor.h"
#include "indication/StatusIndicatorController.h"
#include "input/ButtonInput.h"
#include "input/ButtonInputManager.h"
#include "lighting/LightingController.h"
#include "drivers/Esp32PwmMotor.h"
#include "engine/EngineController.h"


namespace bootstrap {

class ApplicationBootstrap {
public:
    ApplicationBootstrap();

    bool begin(uint32_t now);
    void update(uint32_t now);

private:
    drivers::Button lightButton_;
    drivers::Button brakeButton_;
    drivers::Button turnButton_;
    drivers::Button crashButton_;
    drivers::Button engineButton_;

    input::ButtonInput lightButtonInput_;
    input::ButtonInput brakeButtonInput_;
    input::ButtonInput turnButtonInput_;
    input::ButtonInput crashButtonInput_;
    input::ButtonInput engineButtonInput_;

    input::ButtonInputManager buttonInputManager_;

    drivers::Esp32PwmLed headlights_;
    drivers::Esp32PwmLed brakeLights_;
    drivers::Esp32PwmLed leftTurn_;
    drivers::Esp32PwmLed rightTurn_;

    drivers::Ky040Encoder encoder_;

    app::CarController carController_;
    lighting::LightingController lightingController_;

    drivers::Photoresistor photoresistor_;
    app::AmbientLightController ambientLightController_;

    drivers::AdafruitNeoPixelLed engineLed_;
    drivers::Esp32PwmLed airbagLed_;

    indication::StatusIndicatorController statusIndicatorController_;

    drivers::Esp32PwmMotor motor_;
    engine::EngineController engineController_;

    bool initialized_ = false;
};

}  // namespace bootstrap
