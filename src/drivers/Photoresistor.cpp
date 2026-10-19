#include "drivers/Photoresistor.h"

#include <Arduino.h>

namespace drivers {

Photoresistor::Photoresistor(uint8_t pin) : pin_(pin) {}

void Photoresistor::begin() {
    pinMode(pin_, INPUT);
    analogReadResolution(12);
}

uint16_t Photoresistor::readRaw() {
    return static_cast<uint16_t>(analogRead(pin_));
}

} // namespace drivers
