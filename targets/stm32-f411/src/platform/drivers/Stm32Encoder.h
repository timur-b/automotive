#pragma once

#include <cstdint>

#include "stm32f4xx_hal.h"

#include "input/IEncoderInput.h"
#include "input/QuadratureDecoder.h"

namespace drivers {

class Stm32Encoder final : public IEncoderInput {
public:
    struct Config {
        GPIO_TypeDef* portA;
        uint16_t pinA;

        GPIO_TypeDef* portB;
        uint16_t pinB;

        uint8_t transitionsPerStep = 4;
        bool reverseDirection = false;
    };

    explicit Stm32Encoder(const Config& config);

    bool begin() override;
    bool end() override;

    void update() override;
    int32_t consumeDelta() override;

private:
    static bool enableGpioClock(GPIO_TypeDef* port);

    Config config_;
    QuadratureDecoder decoder_;

    int32_t transitionCount_ = 0;
    int32_t delta_ = 0;

    bool initialized_ = false;
};

} // namespace drivers
