#include "stm32f4xx_hal.h"

#include "platform/bootstrap/ApplicationBootstrap.h"

int main() {
    HAL_Init();

    ApplicationBootstrap application;

    if (!application.begin(HAL_GetTick())) {
        while (true) {
            // Application initialization failed.
        }
    }

    while (true) {
        application.update(HAL_GetTick());
    }
}
