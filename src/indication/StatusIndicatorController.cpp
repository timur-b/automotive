#include "indication/StatusIndicatorController.h"

namespace indication {

namespace {

constexpr uint16_t AIRBAG_ON_BRIGHTNESS = 65535;
constexpr uint16_t AIRBAG_OFF_BRIGHTNESS = 0;

} // namespace

StatusIndicatorController::StatusIndicatorController(
    output::IDigitalOutput& engineLed,
    output::IPwmLed& airbagLed
)
    : engineLed_(engineLed),
      airbagLed_(airbagLed) {}

void StatusIndicatorController::update(
    const domain::VehicleState& state
) {
    if (firstUpdate_ || state.engineState != previousEngineState_) {
        const bool running =
            state.engineState == domain::EngineState::RUNNING;

        engineLed_.set(running);

        previousEngineState_ = state.engineState;
    }

    if (firstUpdate_ || state.airbagsDeployed != previousAirbagsDeployed_) {
        airbagLed_.setBrightness(
            state.airbagsDeployed
                ? AIRBAG_ON_BRIGHTNESS
                : AIRBAG_OFF_BRIGHTNESS
        );

        previousAirbagsDeployed_ = state.airbagsDeployed;
    }

    firstUpdate_ = false;
}

void StatusIndicatorController::reset() {
    firstUpdate_ = true;
    previousEngineState_ = domain::EngineState::OFF;
    previousAirbagsDeployed_ = false;

    engineLed_.set(false);
    airbagLed_.setBrightness(0);
}

} // namespace indication
