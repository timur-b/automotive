#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <cstdint>

namespace drivers {

class Pca9685 {
public:
    Pca9685(TwoWire& wire, uint8_t address, float frequencyHz);
    bool begin();
    bool isInitialized() const;
    bool write(uint8_t channel, uint16_t duty);

private:
    Adafruit_PWMServoDriver driver_;
    float frequencyHz_;
    bool initialized_ = false;
};

} // namespace drivers
