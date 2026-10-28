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

Ky040Encoder::~Ky040Encoder() {
    end();
}

bool Ky040Encoder::begin() {
    if (initialized_) {
        return true;
    }

    if (handlerARegistered_ || handlerBRegistered_) {
        ESP_LOGE(TAG, "Cannot initialize encoder: ISR handlers still registered");
        return false;
    }
    
    gpio_config_t gpioConfig = {};

    gpioConfig.pin_bit_mask =
        (1ULL << config_.pinA) |
        (1ULL << config_.pinB);

    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    esp_err_t err = gpio_config(&gpioConfig);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO initialization failed: %s",
                 esp_err_to_name(err));
        return false;
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

    const gpio_num_t pinA = static_cast<gpio_num_t>(config_.pinA);
    const gpio_num_t pinB = static_cast<gpio_num_t>(config_.pinB);

    err = gpio_isr_handler_add(pinA, &Ky040Encoder::gpioIsrHandler, this);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register GPIO A ISR: %s",
                 esp_err_to_name(err));
        return false;
    }

    handlerARegistered_ = true;

    err = gpio_isr_handler_add(pinB, &Ky040Encoder::gpioIsrHandler, this);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register GPIO B ISR: %s",
                 esp_err_to_name(err));

        if (!end()) {
            ESP_LOGE(TAG, "Encoder rollback failed");
        }

        return false;
    }

    handlerBRegistered_ = true;

    err = gpio_set_intr_type(pinA, GPIO_INTR_ANYEDGE);

    if (err == ESP_OK) {
        err = gpio_set_intr_type(pinB, GPIO_INTR_ANYEDGE);
    }

    if (err == ESP_OK) {
        err = gpio_intr_enable(pinA);
    }

    if (err == ESP_OK) {
        err = gpio_intr_enable(pinB);
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable encoder interrupts: %s",
                 esp_err_to_name(err));

        if (!end()) {
            ESP_LOGE(TAG, "Encoder rollback failed");
        }

        return false;
    }

    initialized_ = true;

    ESP_LOGI(TAG, "Encoder GPIO interrupts registered");

    return true;
}

bool Ky040Encoder::end() {
    const auto pinA = static_cast<gpio_num_t>(config_.pinA);
    const auto pinB = static_cast<gpio_num_t>(config_.pinB);

    bool success = true;

    if (handlerARegistered_) {
        const esp_err_t err = gpio_intr_disable(pinA);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to disable GPIO A interrupt: %s",
                     esp_err_to_name(err));
            success = false;
        }

        const esp_err_t removeErr = gpio_isr_handler_remove(pinA);

        if (removeErr == ESP_OK) {
            handlerARegistered_ = false;
        } else {
            ESP_LOGE(TAG, "Failed to remove GPIO A ISR: %s",
                     esp_err_to_name(removeErr));
            success = false;
        }
    }

    if (handlerBRegistered_) {
        const esp_err_t err = gpio_intr_disable(pinB);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to disable GPIO B interrupt: %s",
                     esp_err_to_name(err));
            success = false;
        }

        const esp_err_t removeErr = gpio_isr_handler_remove(pinB);

        if (removeErr == ESP_OK) {
            handlerBRegistered_ = false;
        } else {
            ESP_LOGE(TAG, "Failed to remove GPIO B ISR: %s",
                     esp_err_to_name(removeErr));
            success = false;
        }
    }

    initialized_ = false;

    return success && !handlerARegistered_ && !handlerBRegistered_;
}

void Ky040Encoder::update() {
    // Encoder state is updated by GPIO interrupts.
}

int32_t Ky040Encoder::consumeDelta() {
    portENTER_CRITICAL(&isrMux_);

    const int32_t result = delta_;
    delta_ = 0;

    portEXIT_CRITICAL(&isrMux_);

    return result;
}

void Ky040Encoder::gpioIsrHandler(void* arg) {
    auto* encoder = static_cast<Ky040Encoder*>(arg);
    encoder->handleInterrupt();
}

void Ky040Encoder::handleInterrupt() {
    const bool a = gpio_get_level(
        static_cast<gpio_num_t>(config_.pinA)
    ) != 0;

    const bool b = gpio_get_level(
        static_cast<gpio_num_t>(config_.pinB)
    ) != 0;

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

} // namespace drivers
