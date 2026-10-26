#include "bootstrap/ApplicationBootstrap.h"

#include "platform/Board.h"
#include "config/LightingConfig.h"
#include "config/AmbientLightConfig.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_err.h"

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

      ledc_(),
      headlights_(
          ledc_,
          Board::HEADLIGHTS_LED,
          Board::HEADLIGHTS_PWM_CHANNEL
      ),
      brakeLights_(
          ledc_,
          Board::BRAKE_LED,
          Board::BRAKE_PWM_CHANNEL
      ),
      leftTurn_(
          ledc_,
          Board::LEFT_TURN_LED,
          Board::LEFT_TURN_PWM_CHANNEL
      ),
      rightTurn_(
          ledc_,
          Board::RIGHT_TURN_LED,
          Board::RIGHT_TURN_PWM_CHANNEL
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

      airbagLed_(
          ledc_,
          Board::AIRBAG_LED,
          Board::AIRBAG_PWM_CHANNEL
      ),

      statusIndicatorController_(
          engineLed_,
          airbagLed_
      ),
      
      motor_(
          ledc_,
          Board::MOTOR_PWM_PIN,
          Board::MOTOR_PWM_CHANNEL
      ),

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

    const esp_err_t err = gpio_install_isr_service(0);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE("BOOTSTRAP",
                "GPIO ISR service initialization failed: %s",
                esp_err_to_name(err));
        return false;
    }

    encoder_.begin();

    if (!buttonInputManager_.add(lightButtonInput_) ||
        !buttonInputManager_.add(brakeButtonInput_) ||
        !buttonInputManager_.add(turnButtonInput_) ||
        !buttonInputManager_.add(crashButtonInput_) ||
        !buttonInputManager_.add(engineButtonInput_)) {
        return false;
    }

    buttonInputManager_.begin(now);

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

    if (!outputsReady) {
        ESP_LOGE("BOOTSTRAP", "PWM outputs initialization failed");
        return false;
    }

    if (!airbagLed_.begin()) {
        ESP_LOGE("BOOTSTRAP", "Airbag LED initialization failed");
        return false;
    }

    if (!engineLed_.begin()) {
        ESP_LOGE("BOOTSTRAP", "RGB LED initialization failed");
        return false;
    }

    if (!motor_.begin()) {
        ESP_LOGE("BOOTSTRAP", "Motor initialization failed");
        return false;
    }

    if (!application_.begin(now)) {
        ESP_LOGE("BOOTSTRAP", "Application initialization failed");
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
