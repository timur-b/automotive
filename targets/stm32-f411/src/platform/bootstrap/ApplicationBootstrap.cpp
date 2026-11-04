#include "ApplicationBootstrap.h"
#include "platform/Board.h"


ApplicationBootstrap::ApplicationBootstrap()
    : tim1_(TIM1, 15, 999),
      tim2_(TIM2, 15, 999),
      tim4_(TIM4, 15, 999),
      headlights_(
          tim4_,
          Board::HEADLIGHTS.channel,
          Board::HEADLIGHTS.port,
          Board::HEADLIGHTS.pin,
          Board::HEADLIGHTS.alternateFunction),
      brakeLights_(
          tim1_,
          Board::BRAKE_LIGHTS.channel,
          Board::BRAKE_LIGHTS.port,
          Board::BRAKE_LIGHTS.pin,
          Board::BRAKE_LIGHTS.alternateFunction),
      leftTurn_(
          tim4_,
          Board::LEFT_TURN.channel,
          Board::LEFT_TURN.port,
          Board::LEFT_TURN.pin,
          Board::LEFT_TURN.alternateFunction),
      rightTurn_(
          tim1_,
          Board::RIGHT_TURN.channel,
          Board::RIGHT_TURN.port,
          Board::RIGHT_TURN.pin,
          Board::RIGHT_TURN.alternateFunction),
      motor_(
          tim2_,
          Board::MOTOR.channel,
          Board::MOTOR.port,
          Board::MOTOR.pin,
          Board::MOTOR.alternateFunction,
          {26, 100}),
      engineIndicator_(
          Board::ENGINE_INDICATOR.port,
          Board::ENGINE_INDICATOR.pin),
      airbagIndicator_(
          Board::AIRBAG_INDICATOR.port,
          Board::AIRBAG_INDICATOR.pin),
      ldr_({
          ADC1,
          Board::LDR.channel,
          Board::LDR.port,
          Board::LDR.pin,
          ADC_SAMPLETIME_480CYCLES
      }),
      lightingController_(
          headlights_,
          brakeLights_,
          leftTurn_,
          rightTurn_,
          lighting::LightingController::Config{}),
      buttons_{{
          {
              Board::ENGINE_BUTTON.port,
              Board::ENGINE_BUTTON.pin,
              drivers::Stm32Button::Polarity::ActiveHigh
          },
          {
              Board::LIGHT_BUTTON.port,
              Board::LIGHT_BUTTON.pin,
              drivers::Stm32Button::Polarity::ActiveHigh
          },
          {
              Board::TURN_BUTTON.port,
              Board::TURN_BUTTON.pin,
              drivers::Stm32Button::Polarity::ActiveHigh
          },
          {
              Board::BRAKE_BUTTON.port,
              Board::BRAKE_BUTTON.pin,
              drivers::Stm32Button::Polarity::ActiveHigh
          },
          {
              Board::CRASH_BUTTON.port,
              Board::CRASH_BUTTON.pin,
              drivers::Stm32Button::Polarity::ActiveHigh
          },
          {
              Board::LOCK_BUTTON.port,
              Board::LOCK_BUTTON.pin,
              drivers::Stm32Button::Polarity::ActiveHigh
          }
      }},
      buttonInputs_{{
          {buttons_[0]},
          {buttons_[1]},
          {buttons_[2]},
          {buttons_[3]},
          {buttons_[4]},
          {buttons_[5]}
      }},
      encoder_({
          Board::ENCODER_CLK.port,
          Board::ENCODER_CLK.pin,
          Board::ENCODER_DT.port,
          Board::ENCODER_DT.pin,
          4,
          false
      }),
      carController_(
          buttonInputs_[static_cast<std::size_t>(ButtonId::Light)],
          buttonInputs_[static_cast<std::size_t>(ButtonId::Brake)],
          buttonInputs_[static_cast<std::size_t>(ButtonId::Turn)],
          buttonInputs_[static_cast<std::size_t>(ButtonId::Crash)],
          buttonInputs_[static_cast<std::size_t>(ButtonId::Engine)],
          encoder_) {
}

bool ApplicationBootstrap::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!tim1_.begin() ||
        !tim2_.begin() ||
        !tim4_.begin() ||
        !headlights_.begin() ||
        !brakeLights_.begin() ||
        !leftTurn_.begin() ||
        !rightTurn_.begin() ||
        !motor_.begin() ||
        !engineIndicator_.begin() ||
        !airbagIndicator_.begin() ||
        !ldr_.begin()) {
        end();
        return false;
    }

    lightingController_.reset();

    for (auto& buttonInput : buttonInputs_) {
        if (!inputManager_.add(buttonInput)) {
            end();
            return false;
        }
    }

    if (!inputManager_.begin(now)) {
        end();
        return false;
    }

    if (!encoder_.begin()) {
        end();
        return false;
    }

    carController_.begin();

    lastLdrSampleAt_ = now;
    ambientDark_ = false;

    uint16_t adcValue = 0;

    if (ldr_.read(adcValue)) {
        ambientDark_ = adcValue < LDR_DARK_THRESHOLD;
    }

    carController_.setAmbientDark(ambientDark_);

    initialized_ = true;
    return true;
}

void ApplicationBootstrap::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    inputManager_.update(now);
    encoder_.update();

    updateAmbientLight(now);

    carController_.update(now);

    const auto& state = carController_.state();

    lightingController_.update(state, now);

    engineIndicator_.set(
        state.engineState == domain::EngineState::RUNNING
    );

    airbagIndicator_.set(state.airbagsDeployed);

    uint8_t motorDuty = 0;

    if (state.engineState == domain::EngineState::RUNNING) {
        constexpr uint8_t MIN_DUTY = 20;
        constexpr uint8_t MAX_DUTY = 40;

        const uint8_t throttle = state.throttlePercent;

        motorDuty = MIN_DUTY +
            (static_cast<uint32_t>(throttle - 50) *
            (MAX_DUTY - MIN_DUTY)) / 50;
    }

    motor_.setDutyPercent(motorDuty);
    motor_.update(now);
}

void ApplicationBootstrap::end() {
    carController_.end();

    encoder_.end();
    inputManager_.end();

    motor_.end();

    ldr_.end();
    airbagIndicator_.end();
    engineIndicator_.end();

    rightTurn_.end();
    leftTurn_.end();
    brakeLights_.end();
    headlights_.end();

    tim4_.end();
    tim2_.end();
    tim1_.end();

    initialized_ = false;
}

void ApplicationBootstrap::updateAmbientLight(uint32_t now) {
    if (static_cast<uint32_t>(now - lastLdrSampleAt_) <
        LDR_SAMPLE_INTERVAL_MS) {
        return;
    }

    lastLdrSampleAt_ = now;

    uint16_t adcValue = 0;

    if (!ldr_.read(adcValue)) {
        return;
    }

    bool dark = ambientDark_;

    if (adcValue < LDR_DARK_THRESHOLD) {
        dark = true;
    } else if (adcValue > LDR_LIGHT_THRESHOLD) {
        dark = false;
    }

    if (dark != ambientDark_) {
        ambientDark_ = dark;
        carController_.setAmbientDark(ambientDark_);
    }
}
