#pragma once

#include "output/IDigitalOutput.h"

#include <cstdint>

namespace drivers {

class GpioOutput final : public output::IDigitalOutput {
public:
    explicit GpioOutput(uint8_t pin, bool activeHigh = true);

    bool begin() override;
    bool end() override;

    void set(bool active) override;

private:
    uint8_t pin_;
    bool activeHigh_;
    bool initialized_ = false;
};

} // namespace drivers
