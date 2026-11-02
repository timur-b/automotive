#pragma once

#include "lighting/LightingController.h"

namespace config {

inline constexpr lighting::LightingController::Config DEFAULT_LIGHTING_CONFIG{
    .blinkIntervalMs = 500,
    .emergencyBrakeBlinkIntervalMs = 125,
    .headlightFadeInMs = 250,
    .headlightFadeOutMs = 350,
    .brakeFadeInMs = 100,
    .brakeFadeOutMs = 180,
    .turnFadeInMs = 120,
    .turnFadeOutMs = 180};

} // namespace config
