/**
 * @file app_main.c
 * @brief Application entry point. Keep it boring.
 *
 * Pattern:
 *  - init NVS if needed
 *  - call initialisation_start()
 *  - optionally run a supervisor loop (or do nothing)
 */

#include "esp_log.h"
#include "initialisation.h"
#include "API_radio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "APP";

void app_main(void)
{
    ESP_LOGI(TAG, "firmware_skeleton_espidf starting");

    esp_err_t err = initialisation_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "initialisation failed: %s", esp_err_to_name(err));
        // In real firmware: enter safe mode / reboot / etc.
        for (;;) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }

    // Demo: periodically send a ping command
    for (;;) {
        (void)API_radio_send_ping();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
