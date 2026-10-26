#pragma once

#include <cstdint>

namespace domain {

enum class EngineState { OFF, RUNNING, STOPPED_BY_CRASH };

enum class HeadlightMode { OFF, ON, AUTO };

enum class TurnSignal { OFF, LEFT, RIGHT, HAZARD };

enum class EmergencyBrakePhase { OFF, RAPID_FLASHING, HAZARD_FLASHING };

struct VehicleState {
    HeadlightMode headlightMode = HeadlightMode::OFF;
    bool ambientDark = false;
    bool brakePressed = false;
    bool crashDetected = false;

    TurnSignal turnSignal = TurnSignal::OFF;

    EmergencyBrakePhase emergencyBrakePhase = EmergencyBrakePhase::OFF;

    EngineState engineState = EngineState::OFF;
    bool airbagsDeployed = false;

    uint8_t throttlePercent = 50;
};

} // namespace domain
