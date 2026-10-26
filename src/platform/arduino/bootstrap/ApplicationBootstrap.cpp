
#include "bootstrap/ApplicationBootstrap.h"

#include "config/AmbientLightConfig.h"
#include "config/LightingConfig.h"
#include "platform/Board.h"

namespace bootstrap {

ApplicationBootstrap::ApplicationBootstrap()
    : lightButton_(Board::LIGHT_BUTTON, drivers::ButtonPolarity::ACTIVE_HIGH),
      brakeButton_(Board::BRAKE_BUTTON, drivers::ButtonPolarity::ACTIVE_HIGH),
      turnButton_(Board::TURN_BUTTON, drivers::ButtonPolarity::ACTIVE_HIGH),
      crashButton_(Board::CRASH_BUTTON, drivers::ButtonPolarity::ACTIVE_HIGH),
      engineButton_(Board::ENGINE_BUTTON, drivers::ButtonPolarity::ACTIVE_LOW),

      lightButtonInput_(lightButton_, Board::DEBOUNCE_MS,
                        {Board::LONG_PRESS_MS, Board::DOUBLE_CLICK_MS}),
      brakeButtonInput_(brakeButton_, Board::DEBOUNCE_MS,
                        {Board::LONG_PRESS_MS, Board::DOUBLE_CLICK_MS}),
      turnButtonInput_(turnButton_, Board::DEBOUNCE_MS,
                       {Board::LONG_PRESS_MS, Board::DOUBLE_CLICK_MS}),
      crashButtonInput_(crashButton_, Board::DEBOUNCE_MS,
                        {Board::LONG_PRESS_MS, Board::DOUBLE_CLICK_MS}),
      engineButtonInput_(engineButton_, Board::DEBOUNCE_MS,
                         {Board::LONG_PRESS_MS, Board::DOUBLE_CLICK_MS}),

      buttonInputManager_(),

      headlights_(Board::HEADLIGHTS_LED, Board::HEADLIGHTS_PWM_CHANNEL),
      brakeLights_(Board::BRAKE_LED, Board::BRAKE_PWM_CHANNEL),
      leftTurn_(Board::LEFT_TURN_LED, Board::LEFT_TURN_PWM_CHANNEL),
      rightTurn_(Board::RIGHT_TURN_LED, Board::RIGHT_TURN_PWM_CHANNEL),

      encoder_({
          Board::ENCODER_A,
          Board::ENCODER_B,
          4,
          false
      }),

      carController_(
          lightButtonInput_,
          brakeButtonInput_,
          turnButtonInput_,
          crashButtonInput_,
          engineButtonInput_,
          encoder_
      ),

      lightingController_(
          headlights_,
          brakeLights_,
          leftTurn_,
          rightTurn_,
          config::DEFAULT_LIGHTING_CONFIG
      ),

      photoresistor_(Board::LDR),

      ambientLightController_(
          photoresistor_,
          config::DEFAULT_AMBIENT_LIGHT_CONFIG
      ),

      engineLed_(Board::RGB_LED),

      airbagLed_(Board::AIRBAG_LED, Board::AIRBAG_PWM_CHANNEL),

      statusIndicatorController_(engineLed_, airbagLed_),

      motor_(Board::MOTOR_PWM_PIN, Board::MOTOR_PWM_CHANNEL),

      engineController_(motor_),

      application_(
          encoder_,
          buttonInputManager_,
          ambientLightController_,
          carController_,
          engineController_,
          lightingController_,
          statusIndicatorController_
      ) {}

bool ApplicationBootstrap::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    // Initialize the encoder.
    encoder_.begin();

    // Register input devices.
    if (!buttonInputManager_.add(lightButtonInput_) ||
        !buttonInputManager_.add(brakeButtonInput_) ||
        !buttonInputManager_.add(turnButtonInput_) ||
        !buttonInputManager_.add(crashButtonInput_) ||
        !buttonInputManager_.add(engineButtonInput_)) {
        return false;
    }

    // Initialize input devices.
    buttonInputManager_.begin(now);

    // Initialize all PWM outputs, even if one fails.
    bool outputsReady = true;

    if (!headlights_.begin()) {
        outputsReady = false;
    }

    if (!brakeLights_.begin()) {
        outputsReady = false;
    }

    if (!leftTurn_.begin()) {
        outputsReady = false;
    }

    if (!rightTurn_.begin()) {
        outputsReady = false;
    }

    if (!engineLed_.begin()) {
        return false;
    }

    if (!airbagLed_.begin()) {
        return false;
    }

    if (!outputsReady) {
        return false;
    }

    // Initialize the motor driver.
    if (!motor_.begin()) {
        return false;
    }

    // Initialize the shared application logic.
    if (!application_.begin(now)) {
        return false;
    }

    initialized_ = true;
    return true;
}

void ApplicationBootstrap::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    application_.update(now);
}

} // namespace bootstrap
