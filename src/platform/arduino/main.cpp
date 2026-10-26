#include <Arduino.h>

#include "bootstrap/ApplicationBootstrap.h"

namespace {

bootstrap::ApplicationBootstrap application;

} // namespace

void setup() {
    Serial.begin(115200);

    const bool initialized = application.begin(millis());

    if (!initialized) {
        Serial.println("ERROR: Application initialization failed.");
        return;
    }

    Serial.println("Application initialized successfully.");
}

void loop() {
    application.update(millis());

    delay(5);
}
