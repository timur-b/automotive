#include "platform/drivers/Stm32AdcInput.h"

namespace drivers {

Stm32AdcInput::Stm32AdcInput(const Config& config)
    : config_(config) {}

bool Stm32AdcInput::enableGpioClock(GPIO_TypeDef* port) {
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else {
        return false;
    }

    return true;
}

bool Stm32AdcInput::begin() {
    if (initialized_) {
        return true;
    }

    if (config_.instance != ADC1 ||
        config_.port == nullptr ||
        config_.pin == 0 ||
        (config_.pin & (config_.pin - 1U)) != 0) {
        return false;
    }

    if (!enableGpioClock(config_.port)) {
        return false;
    }

    __HAL_RCC_ADC1_CLK_ENABLE();

    GPIO_InitTypeDef gpio{};
    gpio.Pin = config_.pin;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(config_.port, &gpio);
    
    resourcesAcquired_ = true;

    adc_.Instance = config_.instance;
    adc_.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc_.Init.Resolution = ADC_RESOLUTION_12B;
    adc_.Init.ScanConvMode = DISABLE;
    adc_.Init.ContinuousConvMode = DISABLE;
    adc_.Init.DiscontinuousConvMode = DISABLE;
    adc_.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_.Init.NbrOfConversion = 1;
    adc_.Init.DMAContinuousRequests = DISABLE;
    adc_.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&adc_) != HAL_OK) {
        end();
        return false;
    }

    ADC_ChannelConfTypeDef channel{};
    channel.Channel = config_.channel;
    channel.Rank = 1;
    channel.SamplingTime = config_.sampleTime;

    if (HAL_ADC_ConfigChannel(&adc_, &channel) != HAL_OK) {
        end();
        return false;
    }

    initialized_ = true;
    return true;
}

bool Stm32AdcInput::read(uint16_t& value) {
    if (!initialized_) {
        return false;
    }

    if (HAL_ADC_Start(&adc_) != HAL_OK) {
        return false;
    }

    const HAL_StatusTypeDef status =
        HAL_ADC_PollForConversion(&adc_, 10);

    if (status != HAL_OK) {
        HAL_ADC_Stop(&adc_);
        return false;
    }

    value = static_cast<uint16_t>(
        HAL_ADC_GetValue(&adc_)
    );

    return HAL_ADC_Stop(&adc_) == HAL_OK;
}

void Stm32AdcInput::end() {
    if (!resourcesAcquired_) {
        return;
    }

    HAL_ADC_DeInit(&adc_);
    HAL_GPIO_DeInit(config_.port, config_.pin);

    __HAL_RCC_ADC1_CLK_DISABLE();

    adc_ = {};
    resourcesAcquired_ = false;
    initialized_ = false;
}

} // namespace drivers
