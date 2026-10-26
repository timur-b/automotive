#include "drivers/Ky040Encoder.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

namespace drivers {

namespace {
constexpr char TAG[] = "Ky040Encoder";
}

Ky040Encoder::Ky040Encoder(const EncoderConfig& config)
    : config_(config) {}

void Ky040Encoder::begin() {
    gpio_config_t gpioConfig = {};

    gpioConfig.pin_bit_mask =
        (1ULL << config_.pinA) |
        (1ULL << config_.pinB);

    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    const esp_err_t err = gpio_config(&gpioConfig);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO initialization failed: %s",
                 esp_err_to_name(err));
        return;
    }

    const bool a = gpio_get_level(
        static_cast<gpio_num_t>(config_.pinA)
    ) != 0;

    const bool b = gpio_get_level(
        static_cast<gpio_num_t>(config_.pinB)
    ) != 0;

    decoder_.reset(a, b);

    transitionCount_ = 0;
    delta_ = 0;
}

void Ky040Encoder::update() {
    const bool a = gpio_get_level(
        static_cast<gpio_num_t>(config_.pinA)
    ) != 0;

    const bool b = gpio_get_level(
        static_cast<gpio_num_t>(config_.pinB)
    ) != 0;

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
}

int32_t Ky040Encoder::consumeDelta() {
    const int32_t result = delta_;
    delta_ = 0;
    return result;
}

} // namespace drivers
