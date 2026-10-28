#include "PwmLed.h"

#include "esp_log.h"

namespace drivers {

namespace {

constexpr char kTag[] = "PwmLed";

} // namespace

PwmLed::PwmLed(
    LedcResourceManager& ledc,
    uint8_t pin,
    uint8_t channel,
    uint32_t frequencyHz,
    uint8_t resolutionBits,
    bool activeHigh)
    : ledc_(ledc),
      pin_(pin),
      channel_(channel),
      frequencyHz_(frequencyHz),
      resolutionBits_(resolutionBits),
      activeHigh_(activeHigh) {}

PwmLed::~PwmLed() {
    if (!end()) {
        ESP_LOGE(kTag, "Failed to release LEDC channel %u",
                 static_cast<unsigned>(channel_));
    }
}

bool PwmLed::begin() {
    if (initialized_) {
        return true;
    }

    const LedcResourceManager::ChannelConfig config{
        pin_,
        static_cast<ledc_channel_t>(channel_),
        LEDC_TIMER_0,
        frequencyHz_,
        static_cast<ledc_timer_bit_t>(resolutionBits_),
        !activeHigh_
    };

    const esp_err_t err = ledc_.registerChannel(config);

    if (err != ESP_OK) {
        ESP_LOGE(kTag, "Failed to register LEDC channel %u: %s",
                 static_cast<unsigned>(channel_),
                 esp_err_to_name(err));
        return false;
    }

    initialized_ = true;
    return true;
}

bool PwmLed::end() {
    if (!initialized_) {
        return true;
    }

    const auto channel = static_cast<ledc_channel_t>(channel_);

    const esp_err_t err = ledc_.unregisterChannel(channel);

    if (err != ESP_OK) {
        ESP_LOGE(kTag, "Failed to unregister LEDC channel %u: %s",
                 static_cast<unsigned>(channel_),
                 esp_err_to_name(err));
        return false;
    }

    initialized_ = false;
    return true;
}

void PwmLed::setBrightness(uint16_t brightness) {
    if (!initialized_) {
        return;
    }

    const uint32_t maxDuty = (1UL << resolutionBits_) - 1UL;

    const uint32_t duty =
        (static_cast<uint32_t>(brightness) * maxDuty + 32767UL)
        / 65535UL;

    const auto channel = static_cast<ledc_channel_t>(channel_);

    const esp_err_t err = ledc_.setDuty(channel, duty);

    if (err != ESP_OK) {
        ESP_LOGE(kTag, "Failed to set LEDC duty: %s",
                 esp_err_to_name(err));
    }
}

} // namespace drivers
