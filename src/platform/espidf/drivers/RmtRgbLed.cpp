#include "drivers/RmtRgbLed.h"

#include "esp_err.h"
#include "esp_log.h"

namespace drivers {

namespace {
constexpr char TAG[] = "RmtRgbLed";
}

RmtRgbLed::RmtRgbLed(uint8_t pin)
    : pin_(pin) {}

RmtRgbLed::~RmtRgbLed() {
    if (!end()) {
        ESP_LOGE(TAG, "Failed to release RGB LED resources");
    }
}

bool RmtRgbLed::begin() {
    if (initialized_) {
        return true;
    }

    led_strip_config_t stripConfig = {};

    stripConfig.strip_gpio_num = static_cast<int>(pin_);
    stripConfig.max_leds = 1;
    stripConfig.led_model = LED_MODEL_WS2812;
    stripConfig.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;

    led_strip_rmt_config_t rmtConfig = {};
    rmtConfig.clk_src = RMT_CLK_SRC_DEFAULT;
    rmtConfig.resolution_hz = 10'000'000;
    rmtConfig.mem_block_symbols = 64;

    esp_err_t err = led_strip_new_rmt_device(
        &stripConfig,
        &rmtConfig,
        &strip_
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "RMT initialization failed: %s",
                 esp_err_to_name(err));
        return false;
    }

    err = led_strip_clear(strip_);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear RGB LED: %s",
                 esp_err_to_name(err));

        led_strip_del(strip_);
        strip_ = nullptr;
        return false;
    }

    initialized_ = true;
    return true;
}

bool RmtRgbLed::end() {
    if (strip_ == nullptr) {
        initialized_ = false;
        return true;
    }

    // Turn off the LED before releasing RMT resources.
    const esp_err_t clearErr = led_strip_clear(strip_);

    if (clearErr != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear RGB LED: %s",
                 esp_err_to_name(clearErr));
    }

    const esp_err_t deleteErr = led_strip_del(strip_);

    if (deleteErr != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete RGB LED strip: %s",
                 esp_err_to_name(deleteErr));
        return false;
    }

    strip_ = nullptr;
    initialized_ = false;

    return clearErr == ESP_OK;
}

void RmtRgbLed::setColor(output::RgbColor color) {
    if (!initialized_) {
        return;
    }

    esp_err_t err = led_strip_set_pixel(
        strip_,
        0,
        color.red,
        color.green,
        color.blue
    );

    if (err == ESP_OK) {
        err = led_strip_refresh(strip_);
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update RGB LED: %s",
                 esp_err_to_name(err));
    }
}

} // namespace drivers
