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
    if (cleanupFailed_) {
        ESP_LOGE("BOOTSTRAP", "Cannot initialize after failed cleanup");
        return false;
    }

    if (initialized_) {
        return true;
    }

    const auto fail = [this](const char* message) -> bool {
        ESP_LOGE("BOOTSTRAP", "%s", message);

        if (!end()) {
            ESP_LOGE("BOOTSTRAP", "Rollback failed");
        }

        return false;
    };

    const esp_err_t err = gpio_install_isr_service(0);

    if (err == ESP_OK) {
        ownsGpioIsrService_ = true;
    } else if (err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE("BOOTSTRAP",
                "GPIO ISR service initialization failed: %s",
                esp_err_to_name(err));
        return fail("GPIO ISR service initialization failed");
    }

    if (!encoder_.begin()) {
        return fail("Encoder initialization failed");
    }

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

    if (!motor_.begin()) {
        return fail("Motor initialization failed");
    }

    if (!application_.begin(now)) {
        return fail("Application initialization failed");
    }

    initialized_ = true;
    return true;
}

bool ApplicationBootstrap::end() {
    ESP_LOGI("BOOTSTRAP", "Bootstrap shutdown started");
    
    initialized_ = false;

    bool success = true;

    // Reset application state and stop handling input events.
    if (!application_.end()) {
        ESP_LOGE("BOOTSTRAP", "Application shutdown failed");
        success = false;
    }

    lightingController_.reset();
    engineController_.reset();
    statusIndicatorController_.reset();

    if (!motor_.end()) {
        ESP_LOGE("BOOTSTRAP", "Motor shutdown failed");
        success = false;
    }

    if (!engineLed_.end()) {
        ESP_LOGE("BOOTSTRAP", "RGB LED shutdown failed");
        success = false;
    }

    if (!airbagLed_.end()) {
        ESP_LOGE("BOOTSTRAP", "Airbag LED shutdown failed");
        success = false;
    }

    if (!rightTurn_.end()) {
        ESP_LOGE("BOOTSTRAP", "Right turn shutdown failed");
        success = false;
    }

    if (!leftTurn_.end()) {
        ESP_LOGE("BOOTSTRAP", "Left turn shutdown failed");
        success = false;
    }

    if (!brakeLights_.end()) {
        ESP_LOGE("BOOTSTRAP", "Brake lights shutdown failed");
        success = false;
    }

    if (!headlights_.end()) {
        ESP_LOGE("BOOTSTRAP", "Headlights shutdown failed");
        success = false;
    }

    if (!buttonInputManager_.end()) {
        ESP_LOGE("ApplicationBootstrap", "Button input shutdown failed");
        success = false;
    }

    if (!encoder_.end()) {
        ESP_LOGE("BOOTSTRAP", "Encoder shutdown failed");
        success = false;
    }

    if (ownsGpioIsrService_) {
        gpio_uninstall_isr_service();
        ownsGpioIsrService_ = false;
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
