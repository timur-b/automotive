
#include "Ky040Encoder.h"

namespace drivers {

Ky040Encoder::Ky040Encoder(const EncoderConfig& config)
    : config_(config) {}

Ky040Encoder::~Ky040Encoder() {
    if (!initialized_) {
        return;
    }

    detachInterrupt(config_.pinA);
    detachInterrupt(config_.pinB);
}

void Ky040Encoder::begin() {
    if (initialized_) {
        return;
    }

    pinMode(config_.pinA, INPUT_PULLUP);
    pinMode(config_.pinB, INPUT_PULLUP);

    const bool a = digitalRead(config_.pinA);
    const bool b = digitalRead(config_.pinB);

    decoder_.reset(a, b);

    transitionCount_ = 0;
    delta_ = 0;

    attachInterruptArg(
        config_.pinA,
        &Ky040Encoder::gpioIsrHandler,
        this,
        CHANGE
    );

    attachInterruptArg(
        config_.pinB,
        &Ky040Encoder::gpioIsrHandler,
        this,
        CHANGE
    );

    initialized_ = true;
}

void Ky040Encoder::update() {
    // Encoder state is updated by GPIO interrupts.
}

void ARDUINO_ISR_ATTR Ky040Encoder::gpioIsrHandler(void* arg) {
    auto* encoder = static_cast<Ky040Encoder*>(arg);
    encoder->handleInterrupt();
}

void ARDUINO_ISR_ATTR Ky040Encoder::handleInterrupt() {
    const bool a = digitalRead(config_.pinA);
    const bool b = digitalRead(config_.pinB);

    portENTER_CRITICAL_ISR(&isrMux_);

    int8_t transition = decoder_.update(a, b);

    if (config_.reverseDirection) {
        transition = -transition;
    }

    transitionCount_ += transition;

    if (transitionCount_ >= config_.transitionsPerStep) {
        ++delta_;
        transitionCount_ -= config_.transitionsPerStep;
    } else if (transitionCount_ <=
               -static_cast<int32_t>(config_.transitionsPerStep)) {
        --delta_;
        transitionCount_ += config_.transitionsPerStep;
    }

    portEXIT_CRITICAL_ISR(&isrMux_);
}

int32_t Ky040Encoder::consumeDelta() {
    portENTER_CRITICAL(&isrMux_);

    const int32_t result = delta_;
    delta_ = 0;

    portEXIT_CRITICAL(&isrMux_);

    return result;
}

} // namespace drivers
