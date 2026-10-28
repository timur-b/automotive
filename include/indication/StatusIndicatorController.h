#pragma once

#include "domain/VehicleState.h"
#include "output/IPwmLed.h"
#include "output/IRgbLed.h"

namespace indication {

class StatusIndicatorController {
public:
    StatusIndicatorController(output::IRgbLed& engineLed, output::IPwmLed& airbagLed);

    void update(const domain::VehicleState& state);

    void reset();
    
private:
    output::IRgbLed& engineLed_;
    output::IPwmLed& airbagLed_;

    bool firstUpdate_ = true;

    domain::EngineState previousEngineState_ = domain::EngineState::OFF;

    bool previousAirbagsDeployed_ = false;
};

} // namespace indication
