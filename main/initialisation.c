/* main/initialisation.c */
#include "initialisation.h"

#include "system_config.h"   // feature flags, stack/prio, sizing
#include "system_cfg.h"      // frozen hardware config (+ optional sanity checks)

#include "system_queues.h"
#include "system_state.h"
#include "system_faults.h"
#include "system_state.h"

#include "task_log.h"
#include "API_display.h"

#if FEATURE_SELFTEST
#include "test_selftest.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "INIT";

static void start_service_tasks(void)
{
    // Add all "service" tasks here (log, watchdog, telemetry, etc.)
    task_log_start();

    // Example:
    // task_watchdog_start();
    // task_cli_start();
}

static void start_application_tasks(void)
{
    // Add your domain tasks here (radio, measure, storage, etc.)
    // task_measure_start();
    // task_radio_start();
    // task_storage_start();
}

void initialisation_start(void)
{
    ESP_LOGI(TAG, "Boot: initialisation_start()");

    // 0) Optional: frozen config sanity checks (if you have them)
    // system_cfg_sanity_check();  // keep it NO FreeRTOS inside

    // 1) Runtime primitives first: queues/event groups/fault bitmap
    system_faults_init();
    system_queues_init();
    system_state_init();

    // 2) Service tasks (infra)
    start_service_tasks();

    // 3) Early boot banner via API (never direct printf from here)
    API_display_boot_banner();

    // 4) Optional self-test (DEV only)
#if FEATURE_SELFTEST
    {
        esp_err_t err = test_selftest_run();
        if (err != ESP_OK) {
            // Decide your policy: halt, reboot, or limp mode.
            // Here: halt hard (explicit).
            ESP_LOGE(TAG, "Selftest failed: %s", esp_err_to_name(err));
            system_faults_set(FAULT_SELFTEST_FAILED);

            // Stop everything: simplest is an infinite loop.
            // (You can also esp_restart() if your policy is reboot-on-fault.)
            for (;;) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }
#endif

    // 5) Application tasks (domain)
    start_application_tasks();

    ESP_LOGI(TAG, "Boot: init complete");
}
