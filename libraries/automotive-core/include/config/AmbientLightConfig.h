#pragma once

#include "app/AmbientLightController.h"

namespace config {

inline constexpr app::AmbientLightController::Config DEFAULT_AMBIENT_LIGHT_CONFIG{
    .sampleIntervalMs = 100,
    .darkThreshold = 1900,
    .brightThreshold = 2100,
    .filterShift = 3,
    .darkerMeansLowerAdc = true};

} // namespace config
