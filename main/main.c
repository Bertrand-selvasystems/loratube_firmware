/* main/main.c (or main/app_main.c depending on your project layout) */

#include "initialisation.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "app_main(): enter");

    // Single entry point: boot orchestration lives in initialisation.*
    initialisation_start();

    // app_main is a task in ESP-IDF. If you don't want it lingering around:
    ESP_LOGI(TAG, "app_main(): done -> deleting self");
    vTaskDelete(NULL);
}
