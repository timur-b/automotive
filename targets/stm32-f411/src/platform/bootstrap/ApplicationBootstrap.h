#pragma once

#include <cstdint>
#include <array>
#include <cstddef>

#include "stm32f4xx_hal.h"

#include "input/ButtonInput.h"
#include "input/ButtonInputManager.h"
#include "app/CarController.h"
#include "lighting/LightingController.h"
#include "platform/drivers/Stm32Button.h"
#include "platform/drivers/Stm32PwmLed.h"
#include "platform/drivers/Stm32PwmTimer.h"
#include "platform/drivers/Stm32Encoder.h"
#include "platform/drivers/Stm32DigitalOutput.h"
#include "platform/drivers/Stm32AdcInput.h"
#include "platform/drivers/Stm32PwmMotor.h"
#include "platform/drivers/Stm32ButtonInterruptController.h"

class ApplicationBootstrap {
public:
    ApplicationBootstrap();

    bool begin(uint32_t now);
    void update(uint32_t now);
    void end();

    void onButtonStateChanged(
        uint16_t pin,
        bool pressed,
        uint32_t timestampMs
    );

    void onButtonExtiInterrupt(uint16_t pin);
    void onButtonTimerInterrupt(uint32_t now);
    void handleButtonTimerIrq();

    uint32_t droppedButtonEvents() const;

private:
    void updateAmbientLight(uint32_t now);
    void recoverButtonInputs(uint32_t now);

    drivers::Stm32PwmTimer tim1_;
    drivers::Stm32PwmTimer tim2_;
    drivers::Stm32PwmTimer tim4_;

    drivers::Stm32PwmLed headlights_;
    drivers::Stm32PwmLed brakeLights_;
    drivers::Stm32PwmLed leftTurn_;
    drivers::Stm32PwmLed rightTurn_;

    drivers::Stm32PwmMotor motor_;

    drivers::Stm32DigitalOutput engineIndicator_;
    drivers::Stm32DigitalOutput airbagIndicator_;
    drivers::Stm32DigitalOutput valetIndicator_;
    drivers::Stm32DigitalOutput lockRelay_;

    drivers::Stm32AdcInput ldr_;

    lighting::LightingController lightingController_;

    enum class ButtonId : std::size_t {
        Engine,
        Light,
        Turn,
        Brake,
        Crash,
        Lock,
        Count
    };

    static constexpr std::size_t BUTTON_COUNT =
        static_cast<std::size_t>(ButtonId::Count);

    std::array<drivers::Stm32Button, BUTTON_COUNT> buttons_;
    std::array<input::ButtonInput, BUTTON_COUNT> buttonInputs_;

    drivers::Stm32Encoder encoder_;
    app::CarController carController_;

    input::ButtonInputManager inputManager_;
    drivers::Stm32ButtonInterruptController buttonInterruptController_;

    static constexpr uint16_t LDR_DARK_THRESHOLD = 1800;
    static constexpr uint16_t LDR_LIGHT_THRESHOLD = 2200;
    static constexpr uint32_t LDR_SAMPLE_INTERVAL_MS = 100;

    uint32_t lastLdrSampleAt_ = 0;
    bool ambientDark_ = false;
    uint32_t lastObservedDroppedEvents_ = 0;

    bool initialized_ = false;
};
