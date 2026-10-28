#include "PwmMotor.h"

#include "esp_log.h"

namespace drivers {

namespace {

constexpr char TAG[] = "PwmMotor";

} // namespace

PwmMotor::PwmMotor(
    LedcResourceManager& ledc,
    uint8_t pin,
    uint8_t channel)
    : ledc_(ledc),
      pin_(pin),
      channel_(channel) {}

PwmMotor::~PwmMotor() {
    if (!end()) {
        ESP_LOGE(TAG, "Failed to release motor resources");
    }
}

bool PwmMotor::begin() {
    if (initialized_) {
        return true;
    }

    const LedcResourceManager::ChannelConfig config{
        pin_,
        static_cast<ledc_channel_t>(channel_),
        LEDC_TIMER_1,
        PWM_FREQUENCY,
        static_cast<ledc_timer_bit_t>(PWM_RESOLUTION),
        false
    };

    const esp_err_t err = ledc_.registerChannel(config);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register motor PWM channel: %s",
                 esp_err_to_name(err));
        return false;
    }

    initialized_ = true;
    return true;
}

bool PwmMotor::end() {
    if (!initialized_) {
        return true;
    }

    const auto channel = static_cast<ledc_channel_t>(channel_);

    const esp_err_t err = ledc_.unregisterChannel(channel);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unregister motor PWM channel: %s",
                 esp_err_to_name(err));
        return false;
    }

    initialized_ = false;
    return true;
}

void PwmMotor::setPower(uint8_t percent) {
    if (!initialized_) {
        return;
    }

    if (percent > 100) {
        percent = 100;
    }

    const uint32_t duty =
        (static_cast<uint32_t>(percent) * MAX_DUTY) / 100U;

    const auto channel = static_cast<ledc_channel_t>(channel_);

    const esp_err_t err = ledc_.setDuty(channel, duty);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set motor PWM duty: %s",
                 esp_err_to_name(err));
    }
}

void PwmMotor::stop() {
    setPower(0);
}

} // namespace drivers
