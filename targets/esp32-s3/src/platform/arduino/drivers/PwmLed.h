#pragma once

#include "output/IPwmLed.h"
#include <cstdint>

namespace drivers {

class PwmLed final : public output::IPwmLed {
public:
    PwmLed(uint8_t pin, uint8_t channel, uint32_t frequencyHz = 5000,
                uint8_t resolutionBits = 12, bool activeHigh = true);

    ~PwmLed();

    bool begin() override;
    bool end() override;
    
    void setBrightness(uint16_t brightness) override;

private:
    uint8_t pin_;
    uint8_t channel_;
    uint32_t frequencyHz_;
    uint8_t resolutionBits_;
    bool activeHigh_;
    bool initialized_ = false;
};

} // namespace drivers
