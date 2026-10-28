#pragma once

#include <cstdint>

#include "drivers/LedcResourceManager.h"
#include "drivers/PwmLed.h"
#include "lighting/LightingController.h"
#include "drivers/Button.h"
#include "input/ButtonInput.h"
#include "input/ButtonInputManager.h"
#include "drivers/Ky040Encoder.h"
#include "app/CarController.h"
#include "drivers/RmtRgbLed.h"
#include "indication/StatusIndicatorController.h"
#include "drivers/PwmMotor.h"
#include "engine/EngineController.h"
#include "drivers/Photoresistor.h"
#include "app/AmbientLightController.h"
#include "app/Application.h"

namespace bootstrap {

class ApplicationBootstrap {
public:
    ApplicationBootstrap();

    bool begin(uint32_t now);
    bool end();
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

    drivers::Ky040Encoder encoder_;

    app::CarController carController_;

    drivers::LedcResourceManager ledc_;

    drivers::PwmLed headlights_;
    drivers::PwmLed brakeLights_;
    drivers::PwmLed leftTurn_;
    drivers::PwmLed rightTurn_;
    
    lighting::LightingController lightingController_;

    drivers::Photoresistor photoresistor_;
    app::AmbientLightController ambientLightController_;

    drivers::RmtRgbLed engineLed_;
    drivers::PwmLed airbagLed_;

    indication::StatusIndicatorController statusIndicatorController_;

    drivers::PwmMotor motor_;
    engine::EngineController engineController_;

    app::Application application_;
    
    bool ownsGpioIsrService_ = false;
    bool initialized_ = false;
    bool cleanupFailed_ = false;
};

} // namespace bootstrap
