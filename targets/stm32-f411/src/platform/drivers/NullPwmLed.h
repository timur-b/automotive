#pragma once

#include "output/IPwmLed.h"

#include <cstdint>

namespace drivers {

class NullPwmLed final : public output::IPwmLed {
public:
    bool begin() override {
        return true;
    }

    bool end() override {
        return true;
    }

    void setBrightness(uint16_t brightness) override {
        (void)brightness;
    }
};

} // namespace drivers
