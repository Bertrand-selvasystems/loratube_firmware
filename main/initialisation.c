/* main/initialisation.c */
#include "initialisation.h"

#include "system_config.h"   // feature flags, stack/prio, sizing
#include "system_cfg.h"      // frozen hardware config (+ optional sanity checks)

#include "system_queues.h"
#include "system_state.h"
#include "system_faults.h"
#include "system_state.h"
#include "system_diag.h"

#include "task_log.h"
#include "API_display.h"
#include "task_i2c.h"
#include "task_pca.h"
#include "task_rtc.h"
#include "API_pca.h"
#include "API_rtc.h"

#define FEATURE_SELFTEST 1


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "INIT";

static void start_service_tasks(void)
{
    // Add all "service" tasks here (log, watchdog, telemetry, etc.)
    task_log_start();

    // I2C dispatcher must exist before any module tries to use I2C.
    task_i2c_start();

    // PCA owner task (depends on I2C owner)
    task_pca_start(CFG_PCA9536_ADDR7);   // ou 0x41, voir plus bas

    task_rtc_start(CFG_PCF8523_ADDR7);

    // Bind API -> PCA queue
    api_pca_bind_queue(task_pca_get_queue());

    // Bind API -> RTC queue
    api_rtc_bind_queue(task_rtc_get_queue());

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


static void rtc_smoke_test(void)
{
    rtc_datetime_t dt = {0};

    // 10 essais max (500 ms) pour laisser la task RTC binder la queue et démarrer.
    for (int i = 0; i < 10; i++) {
        esp_err_t err = api_rtc_get_datetime(&dt, pdMS_TO_TICKS(50));
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "RTC OK: %04u-%02u-%02u %02u:%02u:%02u",
                     dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
            return;
        }
        ESP_LOGW(TAG, "RTC not ready yet: %s", esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Si on arrive ici : ton binding de queue/notify est pas en place.
    ESP_LOGE(TAG, "RTC FAILED (API never answered OK).");
    abort(); // ou return si tu veux continuer en mode dégradé
}


void initialisation_start(void)
{
    ESP_LOGI(TAG, "Boot: initialisation_start()");

    system_diag_boot_dump();

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

        rtc_smoke_test();
            // Stop everything: simplest is an infinite loop.
            // (You can also esp_restart() if your policy is reboot-on-fault.)
    // sécurité : mettre la pin en sortie
    api_pca_set_dir_async(2, true, pdMS_TO_TICKS(50));

    // blink LED1 : 200 ms ON / 200 ms OFF, démarre à 1
    api_pca_blink_start_async(2, 200, 200, true, pdMS_TO_TICKS(50));

    // laisser blinker 5 secondes
    vTaskDelay(pdMS_TO_TICKS(10000));

    // stop blink et éteint la LED
    api_pca_blink_stop_async(1, pdMS_TO_TICKS(50));
    api_pca_set_level_async(1, false, pdMS_TO_TICKS(50));

        }
#endif

    // 5) Application tasks (domain)
    start_application_tasks();

    ESP_LOGI(TAG, "Boot: init complete");
}
