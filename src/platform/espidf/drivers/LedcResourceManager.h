#pragma once

#include <cstdint>

#include "driver/ledc.h"
#include "esp_err.h"

namespace drivers {

class LedcResourceManager {
public:
    struct ChannelConfig {
        uint8_t gpio;
        ledc_channel_t channel;
        ledc_timer_t timer;
        uint32_t frequencyHz;
        ledc_timer_bit_t resolution;
        bool inverted = false;
    };

    LedcResourceManager() = default;

    LedcResourceManager(const LedcResourceManager&) = delete;
    LedcResourceManager& operator=(const LedcResourceManager&) = delete;

    esp_err_t registerChannel(const ChannelConfig& config);

    esp_err_t setDuty(
        ledc_channel_t channel,
        uint32_t duty
    );

private:
    struct TimerState {
        bool configured = false;
        uint32_t frequencyHz = 0;
        ledc_timer_bit_t resolution = LEDC_TIMER_12_BIT;
    };

    struct ChannelState {
        bool allocated = false;
        ledc_timer_t timer = LEDC_TIMER_0;
        uint8_t gpio = 0;
    };

    static constexpr uint8_t kTimerCount = 4;
    static constexpr uint8_t kChannelCount = 8;

    TimerState timers_[kTimerCount]{};
    ChannelState channels_[kChannelCount]{};

    esp_err_t configureTimer(
        ledc_timer_t timer,
        uint32_t frequencyHz,
        ledc_timer_bit_t resolution
    );
};

} // namespace drivers
