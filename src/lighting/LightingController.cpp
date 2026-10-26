#include "lighting/LightingController.h"

namespace lighting {

namespace {

constexpr uint16_t OFF = 0;
constexpr uint16_t FULL = 65535;

} // namespace

void LightingController::Channel::setTarget(uint16_t brightness,
                                            uint32_t durationMs, uint32_t now) {
    if (target == brightness) {
        return;
    }

    // Start a new transition only when the target changes.
    // Otherwise, frequent update() calls would restart the fade.
    target = brightness;
    animator.transitionTo(brightness, durationMs, now);
}

void LightingController::Channel::update(uint32_t now) {
    led.setBrightness(animator.update(now));
}

LightingController::LightingController(output::IPwmLed &headlights,
                                       output::IPwmLed &brakeLights,
                                       output::IPwmLed &leftTurn,
                                       output::IPwmLed &rightTurn,
                                       Config config)
    : headlights_{headlights, {}, 0}, brakeLights_{brakeLights, {}, 0}, leftTurn_{leftTurn, {}, 0},
      rightTurn_{rightTurn, {}, 0}, config_(config) {}

void LightingController::update(const domain::VehicleState &state,
                                uint32_t now) {
    // Brake lights: faster fade in, slower fade out.
    brakeLights_.setTarget(state.brakePressed ? FULL : OFF,
                           state.brakePressed ? config_.brakeFadeInMs
                                              : config_.brakeFadeOutMs,
                           now);

    const bool crash = state.crashDetected;

    const bool emergencyRapid = state.emergencyBrakePhase ==
                                domain::EmergencyBrakePhase::RAPID_FLASHING;

    const bool emergencyHazard = state.emergencyBrakePhase ==
                                 domain::EmergencyBrakePhase::HAZARD_FLASHING;

    const bool emergencyActive = emergencyRapid || emergencyHazard;

    // Reset the blinking phase when the active mode changes.
    if (state.turnSignal != previousTurnSignal_ ||
        crash != previousCrashDetected_ ||
        state.emergencyBrakePhase != previousEmergencyBrakePhase_) {

        previousTurnSignal_ = state.turnSignal;
        previousCrashDetected_ = crash;
        previousEmergencyBrakePhase_ = state.emergencyBrakePhase;

        blinkStartedAt_ = now;
    }

    // Crash mode overrides emergency braking.
    const uint32_t blinkInterval = !crash && emergencyRapid
                                       ? config_.emergencyBrakeBlinkIntervalMs
                                       : config_.blinkIntervalMs;

    const bool blinkingEnabled =
        crash || emergencyActive || state.turnSignal != domain::TurnSignal::OFF;

    bool blinkOn = false;

    if (blinkingEnabled && blinkInterval > 0) {

        const uint32_t elapsed = static_cast<uint32_t>(now - blinkStartedAt_);

        blinkOn = ((elapsed / blinkInterval) % 2U) == 0U;
    }

    const bool leftRequested = crash || emergencyActive ||
                               state.turnSignal == domain::TurnSignal::LEFT ||
                               state.turnSignal == domain::TurnSignal::HAZARD;

    const bool rightRequested = crash || emergencyActive ||
                                state.turnSignal == domain::TurnSignal::RIGHT ||
                                state.turnSignal == domain::TurnSignal::HAZARD;

    const bool leftOn = blinkOn && leftRequested;
    const bool rightOn = blinkOn && rightRequested;

    const bool headlightsRequested =
        state.headlightMode == domain::HeadlightMode::ON ||
        (state.headlightMode == domain::HeadlightMode::AUTO &&
         state.ambientDark);

    const bool headlightsOn =
        state.crashDetected ? blinkOn : headlightsRequested;

    const uint32_t leftFadeMs =
        (crash || emergencyActive)
            ? 0
            : (leftOn ? config_.turnFadeInMs : config_.turnFadeOutMs);

    const uint32_t rightFadeMs =
        (crash || emergencyActive)
            ? 0
            : (rightOn ? config_.turnFadeInMs : config_.turnFadeOutMs);

    leftTurn_.setTarget(leftOn ? FULL : OFF, leftFadeMs, now);

    rightTurn_.setTarget(rightOn ? FULL : OFF, rightFadeMs, now);

    headlights_.setTarget(headlightsOn ? FULL : OFF,
                          crash ? 0
                                : (headlightsOn ? config_.headlightFadeInMs
                                                : config_.headlightFadeOutMs),
                          now);

    // Apply the interpolated brightness to the physical outputs.
    headlights_.update(now);
    brakeLights_.update(now);
    leftTurn_.update(now);
    rightTurn_.update(now);
}

} // namespace lighting
