#pragma once

#include <cstdint>

namespace Board {

constexpr uint8_t LDR = 4;

constexpr uint8_t ENGINE_BUTTON = 12;
constexpr uint8_t LIGHT_BUTTON = 21;
constexpr uint8_t BRAKE_BUTTON = 38;
constexpr uint8_t TURN_BUTTON = 47;
constexpr uint8_t CRASH_BUTTON = 39;

constexpr uint8_t HEADLIGHTS_LED = 13;
constexpr uint8_t BRAKE_LED = 15;
constexpr uint8_t LEFT_TURN_LED = 14;
constexpr uint8_t RIGHT_TURN_LED = 7;
constexpr uint8_t AIRBAG_LED = 6;
constexpr uint8_t RGB_LED = 48;

constexpr uint8_t HEADLIGHTS_PWM_CHANNEL = 0;
constexpr uint8_t BRAKE_PWM_CHANNEL = 1;
constexpr uint8_t LEFT_TURN_PWM_CHANNEL = 2;
constexpr uint8_t RIGHT_TURN_PWM_CHANNEL = 3;
constexpr uint8_t AIRBAG_PWM_CHANNEL = 4;

constexpr uint32_t DEBOUNCE_MS = 30;
constexpr uint32_t LONG_PRESS_MS = 800;
constexpr uint32_t DOUBLE_CLICK_MS = 300;
constexpr uint32_t BLINK_INTERVAL_MS = 500;

} // namespace Board
