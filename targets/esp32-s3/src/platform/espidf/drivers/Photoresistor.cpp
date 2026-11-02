#include "drivers/Photoresistor.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"

namespace drivers {

namespace {
constexpr char TAG[] = "Photoresistor";
}

Photoresistor::Photoresistor(uint8_t pin)
    : pin_(pin) {}

Photoresistor::~Photoresistor() {
    if (!end()) {
        ESP_LOGE(TAG, "Failed to release ADC resources");
    }
}

bool Photoresistor::begin() {
    if (initialized_) {
        return true;
    }

    adc_unit_t unit;
    adc_channel_t channel;

    esp_err_t err = adc_oneshot_io_to_channel(
        static_cast<int>(pin_),
        &unit,
        &channel
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %u is not an ADC pin",
                 static_cast<unsigned>(pin_));
        return false;
    }

    adc_oneshot_unit_init_cfg_t unitConfig = {};
    unitConfig.unit_id = unit;

    err = adc_oneshot_new_unit(&unitConfig, &adcHandle_);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC unit initialization failed: %s",
                 esp_err_to_name(err));
        return false;
    }

    adc_oneshot_chan_cfg_t channelConfig = {};
    channelConfig.bitwidth = ADC_BITWIDTH_12;
    channelConfig.atten = ADC_ATTEN_DB_12;

    err = adc_oneshot_config_channel(
        adcHandle_,
        channel,
        &channelConfig
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC channel configuration failed: %s",
                 esp_err_to_name(err));

        const esp_err_t cleanupErr = adc_oneshot_del_unit(adcHandle_);

        if (cleanupErr == ESP_OK) {
            adcHandle_ = nullptr;
        } else {
            ESP_LOGE(TAG, "ADC cleanup failed: %s",
                     esp_err_to_name(cleanupErr));
        }

        return false;
    }

    channel_ = channel;
    initialized_ = true;

    return true;
}

bool Photoresistor::end() {
    if (adcHandle_ == nullptr) {
        initialized_ = false;
        return true;
    }

    const esp_err_t err = adc_oneshot_del_unit(adcHandle_);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete ADC unit: %s",
                 esp_err_to_name(err));
        return false;
    }

    adcHandle_ = nullptr;
    initialized_ = false;

    return true;
}

uint16_t Photoresistor::readRaw() {
    if (!initialized_) {
        return 0;
    }

    int raw = 0;

    const esp_err_t err = adc_oneshot_read(
        adcHandle_,
        channel_,
        &raw
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed: %s",
                 esp_err_to_name(err));
        return 0;
    }

    return static_cast<uint16_t>(raw);
}

} // namespace drivers
