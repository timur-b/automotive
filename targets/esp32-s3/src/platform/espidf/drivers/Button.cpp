
#include "drivers/Button.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

namespace drivers {

static constexpr char TAG[] = "Button";

Button::Button(uint8_t pin, ButtonPolarity polarity)
    : pin_(pin),
      polarity_(polarity) {}

bool Button::begin() {
    if (initialized_) {
        return true;
    }

    gpio_config_t config = {};

    config.pin_bit_mask = 1ULL << pin_;
    config.mode = GPIO_MODE_INPUT;

    config.pull_up_en = polarity_ == ButtonPolarity::ACTIVE_LOW
                            ? GPIO_PULLUP_ENABLE
                            : GPIO_PULLUP_DISABLE;

    config.pull_down_en = polarity_ == ButtonPolarity::ACTIVE_HIGH
                              ? GPIO_PULLDOWN_ENABLE
                              : GPIO_PULLDOWN_DISABLE;

    config.intr_type = GPIO_INTR_DISABLE;

    const esp_err_t err = gpio_config(&config);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %u initialization failed: %s",
                 static_cast<unsigned>(pin_), esp_err_to_name(err));
        return false;
    }

    initialized_ = true;
    return true;
}

bool Button::end() {
    if (!initialized_) {
        return true;
    }

    const esp_err_t err = gpio_reset_pin(static_cast<gpio_num_t>(pin_));

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %u cleanup failed: %s",
                 static_cast<unsigned>(pin_), esp_err_to_name(err));
        return false;
    }

    initialized_ = false;
    return true;
}

bool Button::isPressed() const {
    if (!initialized_) {
        return false;
    }

    const bool high =
        gpio_get_level(static_cast<gpio_num_t>(pin_)) != 0;

    return polarity_ == ButtonPolarity::ACTIVE_HIGH
               ? high
               : !high;
}

} // namespace drivers
