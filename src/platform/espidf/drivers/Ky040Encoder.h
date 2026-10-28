#pragma once

#include <cstdint>

#include "input/IEncoderInput.h"
#include "input/QuadratureDecoder.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"

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
    static void IRAM_ATTR gpioIsrHandler(void* arg);
    void IRAM_ATTR handleInterrupt();

    EncoderConfig config_;
    QuadratureDecoder decoder_;

    int32_t transitionCount_ = 0;
    int32_t delta_ = 0;

    portMUX_TYPE isrMux_ = portMUX_INITIALIZER_UNLOCKED;

    bool handlerARegistered_ = false;
    bool handlerBRegistered_ = false;
    bool initialized_ = false;
};

} // namespace drivers
