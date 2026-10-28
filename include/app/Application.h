#pragma once

#include <cstdint>

#include "app/AmbientLightController.h"
#include "app/CarController.h"
#include "engine/EngineController.h"
#include "indication/StatusIndicatorController.h"
#include "input/ButtonInputManager.h"
#include "input/IEncoderInput.h"
#include "lighting/LightingController.h"

namespace app {

class Application final {
public:
    Application(
        IEncoderInput& encoder,
        input::ButtonInputManager& buttonInputManager,
        AmbientLightController& ambientLightController,
        CarController& carController,
        engine::EngineController& engineController,
        lighting::LightingController& lightingController,
        indication::StatusIndicatorController& statusIndicatorController
    );

    bool begin(uint32_t now);
    bool end();
    void update(uint32_t now);

private:
    IEncoderInput& encoder_;
    input::ButtonInputManager& buttonInputManager_;
    AmbientLightController& ambientLightController_;
    CarController& carController_;
    engine::EngineController& engineController_;
    lighting::LightingController& lightingController_;
    indication::StatusIndicatorController& statusIndicatorController_;

    bool initialized_ = false;
};

} // namespace app
