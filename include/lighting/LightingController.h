#pragma once

#include <cstdint>

#include "domain/VehicleState.h"
#include "lighting/BrightnessAnimator.h"
#include "output/IPwmLed.h"

namespace lighting {

class LightingController {
public:
    struct Config {
        uint32_t blinkIntervalMs = 500;
        uint32_t emergencyBrakeBlinkIntervalMs = 125;
        uint32_t headlightFadeInMs = 250;
        uint32_t headlightFadeOutMs = 350;
        uint32_t brakeFadeInMs = 100;
        uint32_t brakeFadeOutMs = 180;
        uint32_t turnFadeInMs = 120;
        uint32_t turnFadeOutMs = 180;
    };

    LightingController(output::IPwmLed& headlights, output::IPwmLed& brakeLights,
                       output::IPwmLed& leftTurn, output::IPwmLed& rightTurn, Config config);

    void update(const domain::VehicleState& state, uint32_t now);
    
    void reset();

private:
    struct Channel {
        output::IPwmLed& led;
        BrightnessAnimator animator;
        uint16_t target = 0;

        void setTarget(uint16_t brightness, uint32_t durationMs, uint32_t now);

        void update(uint32_t now);

        void reset();
    };

    Channel headlights_;
    Channel brakeLights_;
    Channel leftTurn_;
    Channel rightTurn_;

    Config config_;

    domain::TurnSignal previousTurnSignal_ = domain::TurnSignal::OFF;

    bool previousCrashDetected_ = false;

    domain::EmergencyBrakePhase previousEmergencyBrakePhase_ = domain::EmergencyBrakePhase::OFF;

    uint32_t blinkStartedAt_ = 0;
};

} // namespace lighting
