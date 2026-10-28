#pragma once

#include <cstdint>

#include "led_strip.h"
#include "output/IRgbLed.h"

namespace drivers {

class RmtRgbLed final : public output::IRgbLed {
public:
    explicit RmtRgbLed(uint8_t pin);
    ~RmtRgbLed() override;

    RmtRgbLed(const RmtRgbLed&) = delete;
    RmtRgbLed& operator=(const RmtRgbLed&) = delete;

    bool begin() override;
    bool end() override;
    
    void setColor(output::RgbColor color) override;

private:
    uint8_t pin_;
    led_strip_handle_t strip_ = nullptr;
    bool initialized_ = false;
};

} // namespace drivers
