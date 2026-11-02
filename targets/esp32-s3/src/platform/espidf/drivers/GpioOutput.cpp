#include "drivers/GpioOutput.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

namespace drivers {

static constexpr char TAG[] = "GpioOutput";

GpioOutput::GpioOutput(uint8_t pin, bool activeHigh)
    : pin_(pin), activeHigh_(activeHigh) {}

bool GpioOutput::begin() {
    if (initialized_) {
        return true;
    }

    const auto gpio = static_cast<gpio_num_t>(pin_);

    // Set the inactive level before enabling output.
    esp_err_t err = gpio_set_level(gpio, activeHigh_ ? 0 : 1);

    if (err == ESP_OK) {
        gpio_config_t config = {};
        config.pin_bit_mask = 1ULL << pin_;
        config.mode = GPIO_MODE_OUTPUT;
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        config.intr_type = GPIO_INTR_DISABLE;

        err = gpio_config(&config);
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %u initialization failed: %s",
                 static_cast<unsigned>(pin_), esp_err_to_name(err));
        return false;
    }

    initialized_ = true;
    return true;
}

bool GpioOutput::end() {
    if (!initialized_) {
        return true;
    }

    set(false);

    const esp_err_t err =
        gpio_reset_pin(static_cast<gpio_num_t>(pin_));

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %u cleanup failed: %s",
                 static_cast<unsigned>(pin_), esp_err_to_name(err));
        return false;
    }

    initialized_ = false;
    return true;
}

void GpioOutput::set(bool active) {
    if (!initialized_) {
        return;
    }

    const bool high = active == activeHigh_;

    gpio_set_level(
        static_cast<gpio_num_t>(pin_),
        high ? 1 : 0
    );
}

} // namespace drivers
