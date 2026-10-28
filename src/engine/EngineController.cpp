#include "engine/EngineController.h"

namespace engine {

EngineController::EngineController(IMotor& motor)
    : motor_(motor) {
}

void EngineController::update(const domain::VehicleState& state) {
    
    const uint8_t requestedPower =
        state.engineState == domain::EngineState::RUNNING
            ? state.throttlePercent
            : 0;

    if (!firstUpdate_ && requestedPower == previousPower_) {
        return;
    }

    if (requestedPower == 0) {
        motor_.stop();
    } else {
        motor_.setPower(requestedPower);
    }

    previousPower_ = requestedPower;
    firstUpdate_ = false;
}

void EngineController::reset() {
    previousPower_ = 0;
    firstUpdate_ = true;
    motor_.stop();
}

} // namespace engine
