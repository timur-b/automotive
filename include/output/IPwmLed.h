#pragma once

#include <cstdint>

namespace output {

class IPwmLed {
public:
    virtual ~IPwmLed() = default;

    virtual bool begin() = 0;
    virtual bool end() = 0;
    
    virtual void setBrightness(uint16_t brightness) = 0;
};

} // namespace output
