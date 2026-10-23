#pragma once

#include "drivers/Pca9685.h"
#include "output/IPwmLed.h"
#include <cstdint>

namespace drivers {

class Pca9685PwmLed final : public output::IPwmLed {
public:
    Pca9685PwmLed(Pca9685& controller, uint8_t channel, bool activeHigh = true);
    bool begin() override;
    void setBrightness(uint16_t brightness) override;

private:
    Pca9685& controller_;
    uint8_t channel_;
    bool activeHigh_;
    bool initialized_ = false;
    bool hasLastDuty_ = false;
    uint16_t lastDuty_ = 0;
};

} // namespace drivers
