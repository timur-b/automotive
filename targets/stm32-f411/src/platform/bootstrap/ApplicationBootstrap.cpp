#include "ApplicationBootstrap.h"

ApplicationBootstrap::ApplicationBootstrap()
    : pwmTimer_(TIM1, 15, 999),
      headlights_(
          pwmTimer_, TIM_CHANNEL_1,
          GPIOA, GPIO_PIN_8, GPIO_AF1_TIM1),
      brakeLights_(
          pwmTimer_, TIM_CHANNEL_2,
          GPIOA, GPIO_PIN_9, GPIO_AF1_TIM1),
      leftTurn_(
          pwmTimer_, TIM_CHANNEL_3,
          GPIOA, GPIO_PIN_10, GPIO_AF1_TIM1),
      rightTurn_(
          pwmTimer_, TIM_CHANNEL_4,
          GPIOA, GPIO_PIN_11, GPIO_AF1_TIM1),
      lightingController_(
          headlights_,
          brakeLights_,
          leftTurn_,
          rightTurn_,
          lighting::LightingController::Config{}),
      button_(GPIOA, GPIO_PIN_0),
      buttonInput_(button_),
      demoController_(buttonInput_, vehicleState_) {
}

bool ApplicationBootstrap::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!pwmTimer_.begin() ||
        !headlights_.begin() ||
        !brakeLights_.begin() ||
        !leftTurn_.begin() ||
        !rightTurn_.begin()) {
        end();
        return false;
    }

    lightingController_.reset();

    inputManager_.add(buttonInput_);

    if (!inputManager_.begin(now)) {
        end();
        return false;
    }

    demoController_.begin();
    
    initialized_ = true;
    return true;
}

void ApplicationBootstrap::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    inputManager_.update(now);
    lightingController_.update(vehicleState_, now);
}

void ApplicationBootstrap::end() {
    demoController_.end();
    rightTurn_.end();
    leftTurn_.end();
    brakeLights_.end();
    headlights_.end();
    pwmTimer_.end();

    initialized_ = false;
}
