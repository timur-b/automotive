#include "Photoresistor.h"

#include <Arduino.h>

namespace drivers {

Photoresistor::Photoresistor(uint8_t pin) : pin_(pin) {}

bool Photoresistor::begin() {
    pinMode(pin_, INPUT);
    analogReadResolution(12);
    return true;
}

bool Photoresistor::end() {
    return true;
}

uint16_t Photoresistor::readRaw() {
    return static_cast<uint16_t>(analogRead(pin_));
}

} // namespace drivers
