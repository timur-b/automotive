#include "app/Application.h"

namespace app {

Application::Application(
    IEncoderInput& encoder,
    input::ButtonInputManager& buttonInputManager,
    AmbientLightController& ambientLightController,
    CarController& carController,
    engine::EngineController& engineController,
    lighting::LightingController& lightingController,
    indication::StatusIndicatorController& statusIndicatorController)
    : encoder_(encoder),
      buttonInputManager_(buttonInputManager),
      ambientLightController_(ambientLightController),
      carController_(carController),
      engineController_(engineController),
      lightingController_(lightingController),
      statusIndicatorController_(statusIndicatorController) {}

bool Application::begin(uint32_t now) {
    if (initialized_) {
        return true;
    }

    if (!ambientLightController_.begin(now)) {
        return false;
    }

    engineController_.update(carController_.state());

    // Register callbacks and synchronize the initial brake state.
    carController_.begin();

    carController_.setAmbientDark(ambientLightController_.isDark());

    statusIndicatorController_.update(carController_.state());

    // Apply the initial lighting state.
    lightingController_.update(carController_.state(), now);

    initialized_ = true;
    return true;
}

bool Application::end() {
    initialized_ = false;

    // Stop handling vehicle events and reset its logical state.
    carController_.end();

    // Release resources owned by the ambient light subsystem.
    return ambientLightController_.end();
}

void Application::update(uint32_t now) {
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

} // namespace app
