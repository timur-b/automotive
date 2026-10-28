
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
    if (cleanupFailed_) {
        Serial.println("Cannot initialize after failed cleanup");
        return false;
    }

    if (initialized_) {
        return true;
    }

    const auto fail = [this](const char* message) -> bool {
        Serial.println(message);

        if (!end()) {
            Serial.println("Rollback failed");
        }

        return false;
    };

    // Initialize the encoder.
    if (!encoder_.begin()) {
        return fail("Encoder initialization failed");
    }

    // Register input devices.
    if (!buttonInputManager_.add(lightButtonInput_) ||
        !buttonInputManager_.add(brakeButtonInput_) ||
        !buttonInputManager_.add(turnButtonInput_) ||
        !buttonInputManager_.add(crashButtonInput_) ||
        !buttonInputManager_.add(engineButtonInput_)) {
        return fail("Button input registration failed");
    }

     if (!buttonInputManager_.begin(now)) {
        return fail("Button input initialization failed");
    }

    // Initialize PWM outputs.
    if (!headlights_.begin()) {
        return fail("Headlights initialization failed");
    }

    if (!brakeLights_.begin()) {
        return fail("Brake lights initialization failed");
    }

    if (!leftTurn_.begin()) {
        return fail("Left turn initialization failed");
    }

    if (!rightTurn_.begin()) {
        return fail("Right turn initialization failed");
    }

    if (!airbagLed_.begin()) {
        return fail("Airbag LED initialization failed");
    }

    if (!engineLed_.begin()) {
        return fail("RGB LED initialization failed");
    }

    // Initialize the motor driver.
    if (!motor_.begin()) {
        return fail("Motor initialization failed");
    }

    // Initialize the shared application logic.
    if (!application_.begin(now)) {
        return fail("Application initialization failed");
    }

    initialized_ = true;
    return true;
}

bool ApplicationBootstrap::end() {
    Serial.println("Bootstrap shutdown started");

    initialized_ = false;
    bool success = true;

    // Stop the application and reset its controllers.
    if (!application_.end()) {
        Serial.println("Application shutdown failed");
        success = false;
    }

    lightingController_.reset();
    engineController_.reset();
    statusIndicatorController_.reset();

    // Stop the motor and release PWM resources.
    if (!motor_.end()) {
        Serial.println("Motor shutdown failed");
        success = false;
    }

    // Turn off the RGB LED.
    if (!engineLed_.end()) {
        Serial.println("RGB LED shutdown failed");
        success = false;
    }

    // Release LED PWM outputs.
    if (!airbagLed_.end()) {
        Serial.println("Airbag LED shutdown failed");
        success = false;
    }

    if (!rightTurn_.end()) {
        Serial.println("Right turn shutdown failed");
        success = false;
    }

    if (!leftTurn_.end()) {
        Serial.println("Left turn shutdown failed");
        success = false;
    }

    if (!brakeLights_.end()) {
        Serial.println("Brake lights shutdown failed");
        success = false;
    }

    if (!headlights_.end()) {
        Serial.println("Headlights shutdown failed");
        success = false;
    }

    // Stop button processing and detach encoder interrupts.
    if (!buttonInputManager_.end()) {
        Serial.println("Button input shutdown failed");
        success = false;
    }

    if (!encoder_.end()) {
        Serial.println("Encoder shutdown failed");
        success = false;
    }

    cleanupFailed_ = !success;
    return success;
}

void ApplicationBootstrap::update(uint32_t now) {
    if (!initialized_) {
        return;
    }

    application_.update(now);
}

} // namespace bootstrap
