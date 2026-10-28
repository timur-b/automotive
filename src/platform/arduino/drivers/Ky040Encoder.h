#pragma once

#include <Arduino.h>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "input/IEncoderInput.h"
#include "input/QuadratureDecoder.h"

namespace drivers {

struct EncoderConfig {
    uint8_t pinA;
    uint8_t pinB;
    uint8_t transitionsPerStep = 4;
    bool reverseDirection = false;
};

class Ky040Encoder final : public IEncoderInput {
public:
    explicit Ky040Encoder(const EncoderConfig& config);
    ~Ky040Encoder() override;

    Ky040Encoder(const Ky040Encoder&) = delete;
    Ky040Encoder& operator=(const Ky040Encoder&) = delete;

    bool begin() override;
    bool end() override;

    void update() override;
    int32_t consumeDelta() override;

private:
    static void ARDUINO_ISR_ATTR gpioIsrHandler(void* arg);
    void ARDUINO_ISR_ATTR handleInterrupt();

    EncoderConfig config_;
    QuadratureDecoder decoder_;

    portMUX_TYPE isrMux_ = portMUX_INITIALIZER_UNLOCKED;

    int32_t transitionCount_ = 0;
    int32_t delta_ = 0;

    bool initialized_ = false;
};

} // namespace drivers
