#pragma once

#include "sensors/ILightSensor.h"
#include <cstdint>

namespace drivers {

class Photoresistor : public input::ILightSensor {
public:
    explicit Photoresistor(uint8_t pin);

    void begin() override;
    uint16_t readRaw() override;

private:
    uint8_t pin_;
};

} // namespace drivers
