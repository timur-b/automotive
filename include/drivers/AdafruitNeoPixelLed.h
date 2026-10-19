#pragma once

#include <Adafruit_NeoPixel.h>
#include <cstdint>

#include "output/IRgbLed.h"

namespace drivers {

class AdafruitNeoPixelLed : public output::IRgbLed {
public:
    explicit AdafruitNeoPixelLed(uint8_t pin);

    bool begin() override;
    void setColor(output::RgbColor color) override;

private:
    Adafruit_NeoPixel pixel_;
    bool initialized_ = false;
};

} // namespace drivers
