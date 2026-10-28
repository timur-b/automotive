#pragma once

#include <cstdint>

#include "esp_adc/adc_oneshot.h"
#include "sensors/ILightSensor.h"

namespace drivers {

class Photoresistor final : public input::ILightSensor {
public:
    explicit Photoresistor(uint8_t pin);
    ~Photoresistor() override;

    Photoresistor(const Photoresistor&) = delete;
    Photoresistor& operator=(const Photoresistor&) = delete;

    bool begin() override;
    bool end() override;
    uint16_t readRaw() override;

private:
    uint8_t pin_;

    adc_oneshot_unit_handle_t adcHandle_ = nullptr;
    adc_channel_t channel_ = ADC_CHANNEL_0;

    bool initialized_ = false;
};

} // namespace drivers
