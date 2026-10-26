#include <cstdint>

#include "bootstrap/ApplicationBootstrap.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr char TAG[] = "APP";

uint32_t millis() {
    return static_cast<uint32_t>(
        esp_timer_get_time() / 1000ULL
    );
}

} // namespace

extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting application");

    static bootstrap::ApplicationBootstrap application;

    if (!application.begin(millis())) {
        ESP_LOGE(TAG, "Application initialization failed");
        return;
    }

    ESP_LOGI(TAG, "Application initialized successfully");

    while (true) {
        application.update(millis());

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
