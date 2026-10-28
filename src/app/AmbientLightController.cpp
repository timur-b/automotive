#include "app/AmbientLightController.h"

namespace app {

AmbientLightController::AmbientLightController(input::ILightSensor& sensor, Config config)
    : sensor_(sensor), config_(config) {}

bool AmbientLightController::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!sensor_.begin()) {
        return false;
    }

    const uint16_t initial = sensor_.readRaw();

    filteredScaled_ =
        static_cast<uint32_t>(initial) << config_.filterShift;

    lastSampleAt_ = now;

    const uint16_t threshold = static_cast<uint16_t>(
        (static_cast<uint32_t>(config_.darkThreshold) +
         config_.brightThreshold) / 2U
    );

    dark_ = config_.darkerMeansLowerAdc
        ? initial <= threshold
        : initial >= threshold;

    initialized_ = true;

    return true;
}

bool AmbientLightController::end() {
    const bool success = sensor_.end();

    if (!success) {
        return false;
    }

    initialized_ = false;
    filteredScaled_ = 0;
    lastSampleAt_ = 0;
    dark_ = false;

    return true;
}

void AmbientLightController::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    if (static_cast<uint32_t>(now - lastSampleAt_) < config_.sampleIntervalMs) {
        return;
    }

    lastSampleAt_ = now;

    const uint16_t raw = sensor_.readRaw();

    // Apply an exponential moving average using fixed-point arithmetic.
    const uint32_t target = static_cast<uint32_t>(raw) << config_.filterShift;

    if (target >= filteredScaled_) {
        filteredScaled_ += (target - filteredScaled_) >> config_.filterShift;
    } else {
        filteredScaled_ -= (filteredScaled_ - target) >> config_.filterShift;
    }

    const uint16_t filtered = filteredValue();

    // Apply hysteresis to prevent rapid state changes.
    if (config_.darkerMeansLowerAdc) {
        if (!dark_ && filtered <= config_.darkThreshold) {
            dark_ = true;
        } else if (dark_ && filtered >= config_.brightThreshold) {
            dark_ = false;
        }
    } else {
        if (!dark_ && filtered >= config_.brightThreshold) {
            dark_ = true;
        } else if (dark_ && filtered <= config_.darkThreshold) {
            dark_ = false;
        }
    }
}

bool AmbientLightController::isDark() const {
    return dark_;
}

uint16_t AmbientLightController::filteredValue() const {
    return static_cast<uint16_t>(filteredScaled_ >> config_.filterShift);
}

}  // namespace app
