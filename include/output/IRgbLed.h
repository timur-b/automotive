#pragma once

#include <cstdint>

namespace output {

struct RgbColor {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

class IRgbLed {
public:
    virtual ~IRgbLed() = default;

    virtual bool begin() = 0;
    virtual void setColor(RgbColor color) = 0;
};

} // namespace output
