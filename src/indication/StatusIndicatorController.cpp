#include "indication/StatusIndicatorController.h"

namespace indication {

namespace {

constexpr output::RgbColor ENGINE_RUNNING_COLOR{0, 10, 0};

constexpr output::RgbColor ENGINE_OFF_COLOR{0, 0, 0};

constexpr uint16_t AIRBAG_ON_BRIGHTNESS = 65535;
constexpr uint16_t AIRBAG_OFF_BRIGHTNESS = 0;

} // namespace

StatusIndicatorController::StatusIndicatorController(output::IRgbLed& engineLed,
                                                     output::IPwmLed& airbagLed)
    : engineLed_(engineLed), airbagLed_(airbagLed) {}

void StatusIndicatorController::update(const domain::VehicleState& state) {
    if (firstUpdate_ || state.engineState != previousEngineState_) {

        const bool running = state.engineState == domain::EngineState::RUNNING;

        engineLed_.setColor(running ? ENGINE_RUNNING_COLOR : ENGINE_OFF_COLOR);

        previousEngineState_ = state.engineState;
    }

    if (firstUpdate_ || state.airbagsDeployed != previousAirbagsDeployed_) {

        airbagLed_.setBrightness(state.airbagsDeployed ? AIRBAG_ON_BRIGHTNESS
                                                       : AIRBAG_OFF_BRIGHTNESS);

        previousAirbagsDeployed_ = state.airbagsDeployed;
    }

    firstUpdate_ = false;
}

} // namespace indication
