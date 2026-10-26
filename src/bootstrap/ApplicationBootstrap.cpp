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

      carController_(lightButtonInput_, brakeButtonInput_, turnButtonInput_, crashButtonInput_,
                     engineButtonInput_, encoder_),

      lightingController_(headlights_, brakeLights_, leftTurn_, rightTurn_,
                          config::DEFAULT_LIGHTING_CONFIG),

      photoresistor_(Board::LDR),
      ambientLightController_(photoresistor_, config::DEFAULT_AMBIENT_LIGHT_CONFIG),

      engineLed_(Board::RGB_LED),

      airbagLed_(Board::AIRBAG_LED, Board::AIRBAG_PWM_CHANNEL),

      statusIndicatorController_(engineLed_, airbagLed_),

      motor_(Board::MOTOR_PWM_PIN, Board::MOTOR_PWM_CHANNEL),
      engineController_(motor_) {}

bool ApplicationBootstrap::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    encoder_.begin();

    // Register input devices.
    if (!buttonInputManager_.add(lightButtonInput_) ||
        !buttonInputManager_.add(brakeButtonInput_) || !buttonInputManager_.add(turnButtonInput_) ||
        !buttonInputManager_.add(crashButtonInput_) ||
        !buttonInputManager_.add(engineButtonInput_)) {
        return false;
    }

    // Initialize inputs.
    buttonInputManager_.begin(now);

    // Initialize PWM outputs.
    // Initialize every output even if a previous one fails.
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

    if (!motor_.begin()) {
        return false;
    }

    engineController_.update(carController_.state());

    // Register callbacks and synchronize the initial brake state.
    carController_.begin();

    ambientLightController_.begin(now);

    carController_.setAmbientDark(ambientLightController_.isDark());

    statusIndicatorController_.update(carController_.state());

    // Apply the initial lighting state.
    lightingController_.update(carController_.state(), now);

    initialized_ = true;

    return true;
}

void ApplicationBootstrap::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    encoder_.update();

    buttonInputManager_.update(now);

    ambientLightController_.update(now);

    carController_.setAmbientDark(ambientLightController_.isDark());

    carController_.update(now);

    engineController_.update(carController_.state());

    lightingController_.update(carController_.state(), now);

    statusIndicatorController_.update(carController_.state());
}

} // namespace bootstrap
