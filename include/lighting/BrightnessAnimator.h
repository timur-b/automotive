#pragma once

#include <cstdint>

namespace lighting {

class BrightnessAnimator {
public:
    void reset(uint16_t brightness);

    void transitionTo(uint16_t target, uint32_t durationMs, uint32_t now);

    uint16_t update(uint32_t now);
    uint16_t value() const;

private:
    uint16_t current_ = 0;
    uint16_t start_ = 0;
    uint16_t target_ = 0;

    uint32_t startedAt_ = 0;
    uint32_t durationMs_ = 0;
};

} // namespace lighting
