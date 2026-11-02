#include "LedcResourceManager.h"
#include "driver/gpio.h"

namespace drivers {

esp_err_t LedcResourceManager::configureTimer(
    ledc_timer_t timer,
    uint32_t frequencyHz,
    ledc_timer_bit_t resolution
) {
    const auto index = static_cast<uint8_t>(timer);

    if (index >= kTimerCount ||
        frequencyHz == 0 ||
        resolution < LEDC_TIMER_1_BIT ||
        resolution > LEDC_TIMER_14_BIT) {
        return ESP_ERR_INVALID_ARG;
    }

    auto& state = timers_[index];

    if (state.configured) {
        const bool sameConfiguration =
            state.frequencyHz == frequencyHz &&
            state.resolution == resolution;

        if (sameConfiguration) {
            return ESP_OK;
        }

        if (state.users > 0) {
            return ESP_ERR_INVALID_STATE;
        }
    }

    ledc_timer_config_t config = {};

    config.speed_mode = LEDC_LOW_SPEED_MODE;
    config.timer_num = timer;
    config.duty_resolution = resolution;
    config.freq_hz = frequencyHz;
    config.clk_cfg = LEDC_AUTO_CLK;

    const esp_err_t result = ledc_timer_config(&config);

    if (result != ESP_OK) {
        return result;
    }

    state.configured = true;
    state.frequencyHz = frequencyHz;
    state.resolution = resolution;

    return ESP_OK;
}

esp_err_t LedcResourceManager::registerChannel(
    const ChannelConfig& config
) {
    const auto channelIndex =
        static_cast<uint8_t>(config.channel);

    const auto timerIndex =
        static_cast<uint8_t>(config.timer);

    if (channelIndex >= kChannelCount ||
        timerIndex >= kTimerCount) {
        return ESP_ERR_INVALID_ARG;
    }

    if (channels_[channelIndex].allocated) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t result = configureTimer(
        config.timer,
        config.frequencyHz,
        config.resolution
    );

    if (result != ESP_OK) {
        return result;
    }

    ledc_channel_config_t channelConfig = {};

    channelConfig.gpio_num = config.gpio;
    channelConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    channelConfig.channel = config.channel;
    channelConfig.timer_sel = config.timer;
    channelConfig.duty = 0;
    channelConfig.hpoint = 0;
    channelConfig.flags.output_invert = config.inverted;

    result = ledc_channel_config(&channelConfig);

    if (result != ESP_OK) {
        return result;
    }

    auto& state = channels_[channelIndex];

    state.allocated = true;
    state.timer = config.timer;
    state.gpio = config.gpio;

    ++timers_[timerIndex].users;

    return ESP_OK;
}

esp_err_t LedcResourceManager::unregisterChannel(
    ledc_channel_t channel
) {
    const auto index = static_cast<uint8_t>(channel);

    if (index >= kChannelCount) {
        return ESP_ERR_INVALID_ARG;
    }

    auto& state = channels_[index];

    if (!state.allocated) {
        return ESP_OK;
    }

    // Stop PWM generation and drive the output LOW.
    esp_err_t result = ledc_stop(
        LEDC_LOW_SPEED_MODE,
        channel,
        0
    );

    if (result != ESP_OK) {
        return result;
    }

    const auto gpio = static_cast<gpio_num_t>(state.gpio);

    // Reset the GPIO configuration and detach its peripheral routing.
    result = gpio_reset_pin(gpio);

    if (result != ESP_OK) {
        return result;
    }

    // Prepare LOW level before enabling the GPIO output driver.
    result = gpio_set_level(gpio, 0);

    if (result != ESP_OK) {
        return result;
    }

    // Keep the MOSFET gate actively driven LOW.
    result = gpio_set_direction(gpio, GPIO_MODE_OUTPUT);

    if (result != ESP_OK) {
        return result;
    }

    // Release the channel and update the timer reference count.
    const auto timerIndex = static_cast<uint8_t>(state.timer);
    auto& timer = timers_[timerIndex];

    state.allocated = false;

    if (timer.users > 0) {
        --timer.users;
    }

    return ESP_OK;
}

esp_err_t LedcResourceManager::setDuty(
    ledc_channel_t channel,
    uint32_t duty
) {
    const auto index = static_cast<uint8_t>(channel);

    if (index >= kChannelCount) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!channels_[index].allocated) {
        return ESP_ERR_INVALID_STATE;
    }

    const auto timerIndex =
        static_cast<uint8_t>(channels_[index].timer);

    const auto resolution = timers_[timerIndex].resolution;
    const uint32_t maxDuty =
        (1UL << static_cast<unsigned>(resolution)) - 1UL;

    if (duty > maxDuty) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t result = ledc_set_duty(
        LEDC_LOW_SPEED_MODE,
        channel,
        duty
    );

    if (result != ESP_OK) {
        return result;
    }

    return ledc_update_duty(
        LEDC_LOW_SPEED_MODE,
        channel
    );
}

} // namespace drivers
