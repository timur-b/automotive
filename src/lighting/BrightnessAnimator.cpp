#include "lighting/BrightnessAnimator.h"

namespace lighting {

void BrightnessAnimator::reset(uint16_t brightness) {
    current_ = brightness;
    start_ = brightness;
    target_ = brightness;
    startedAt_ = 0;
    durationMs_ = 0;
}

void BrightnessAnimator::transitionTo(uint16_t target, uint32_t durationMs,
                                      uint32_t now) {
    // Capture the current interpolated brightness before
    // starting a new transition.
    update(now);

    start_ = current_;
    target_ = target;
    startedAt_ = now;
    durationMs_ = durationMs;

    if (durationMs_ == 0) {
        current_ = target_;
    }
}

uint16_t BrightnessAnimator::update(uint32_t now) {
    if (durationMs_ == 0) {
        return current_;
    }

    const uint32_t elapsed = static_cast<uint32_t>(now - startedAt_);

    if (elapsed >= durationMs_) {
        current_ = target_;
        durationMs_ = 0;
        return current_;
    }

    const int32_t difference =
        static_cast<int32_t>(target_) - static_cast<int32_t>(start_);

    const int32_t interpolated =
        static_cast<int32_t>(start_) +
        static_cast<int32_t>((static_cast<int64_t>(difference) * elapsed) /
                             durationMs_);

    current_ = static_cast<uint16_t>(interpolated);

    return current_;
}

uint16_t BrightnessAnimator::value() const { return current_; }

} // namespace lighting
