#pragma once

#include <cstdint>

#include "stm32f4xx_hal.h"

namespace drivers {

class Stm32AdcInput final {
public:
    struct Config {
        ADC_TypeDef* instance;
        uint32_t channel;
        GPIO_TypeDef* port;
        uint16_t pin;
        uint32_t sampleTime;
    };

    explicit Stm32AdcInput(const Config& config);

    bool begin();
    bool read(uint16_t& value);
    void end();

private:
    static bool enableGpioClock(GPIO_TypeDef* port);

    Config config_;
    ADC_HandleTypeDef adc_{};
    
    bool resourcesAcquired_ = false;
    bool initialized_ = false;
};

} // namespace drivers
