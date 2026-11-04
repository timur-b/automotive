#pragma once

#include "stm32f4xx_hal.h"

namespace Board {

struct DigitalPin {
    GPIO_TypeDef* port;
    uint16_t pin;
};

struct PwmPin {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t channel;
    uint8_t alternateFunction;
};

struct AdcPin {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t channel;
};

// PWM outputs

inline const PwmPin HEADLIGHTS = {
    GPIOB, GPIO_PIN_6, TIM_CHANNEL_1, GPIO_AF2_TIM4
};

inline const PwmPin BRAKE_LIGHTS = {
    GPIOA, GPIO_PIN_10, TIM_CHANNEL_3, GPIO_AF1_TIM1
};

inline const PwmPin LEFT_TURN = {
    GPIOB, GPIO_PIN_7, TIM_CHANNEL_2, GPIO_AF2_TIM4
};

inline const PwmPin RIGHT_TURN = {
    GPIOA, GPIO_PIN_9, TIM_CHANNEL_2, GPIO_AF1_TIM1
};

// Motor PWM output.
//
// Current hardware mapping:
//   GPIO:  PB10
//   Timer: TIM2
//   CH:    TIM2_CH3
//   AF:    AF1
//
// Stm32PwmMotor currently validates this exact mapping
// during begin(). Changing Board::MOTOR alone is not
// sufficient to remap the motor.
//
// The motor is driven through an external transistor,
// not directly from the MCU GPIO.

inline const PwmPin MOTOR = {
    GPIOB, GPIO_PIN_10, TIM_CHANNEL_3, GPIO_AF1_TIM2
};

// Digital outputs

inline const DigitalPin ENGINE_INDICATOR = {
    GPIOB, GPIO_PIN_12
};

inline const DigitalPin AIRBAG_INDICATOR = {
    GPIOB, GPIO_PIN_13
};

// Reserved for future use.
inline const DigitalPin LOCK_INDICATOR = {
    GPIOA, GPIO_PIN_8
};

// Buttons (active-high, internal pull-down)

inline const DigitalPin ENGINE_BUTTON = {
    GPIOA, GPIO_PIN_1
};

inline const DigitalPin LIGHT_BUTTON = {
    GPIOA, GPIO_PIN_2
};

inline const DigitalPin TURN_BUTTON = {
    GPIOA, GPIO_PIN_3
};

inline const DigitalPin BRAKE_BUTTON = {
    GPIOA, GPIO_PIN_4
};

inline const DigitalPin CRASH_BUTTON = {
    GPIOA, GPIO_PIN_5
};

inline const DigitalPin LOCK_BUTTON = {
    GPIOA, GPIO_PIN_6
};

// Quadrature encoder (KY-040)

inline const DigitalPin ENCODER_CLK = {
    GPIOB, GPIO_PIN_1
};

inline const DigitalPin ENCODER_DT = {
    GPIOB, GPIO_PIN_0
};

// Ambient light sensor (ADC1)

inline const AdcPin LDR = {
    GPIOA, GPIO_PIN_7, ADC_CHANNEL_7
};

} // namespace Board
