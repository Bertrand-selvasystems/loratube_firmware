/**
 * @file initialisation.c
 * @brief Boot orchestration implementation.
 */

#include "initialisation.h"

#include "esp_log.h"
#include "esp_check.h"

#include "system_config.h"
#include "system_state.h"
#include "system_queues.h"
#include "system_faults.h"
#include "system_irq.h"

#include "task_measure.h"
#include "task_radio.h"

#if FEATURE_SELFTEST
#include "test_selftest.h"
#endif

static const char *TAG = "INIT";

system_t sys = {
    .cfg = &SYS_CFG,
    .init_done = false,
};

esp_err_t initialisation_start(void)
{
    ESP_LOGI(TAG, "boot start");

    ESP_RETURN_ON_ERROR(system_state_init(),  TAG, "system_state_init");
    ESP_RETURN_ON_ERROR(system_faults_init(), TAG, "system_faults_init");
    ESP_RETURN_ON_ERROR(system_queues_init(), TAG, "system_queues_init");
    system_irq_init();

#if FEATURE_SELFTEST
    ESP_LOGW(TAG, "FEATURE_SELFTEST enabled (DEV build)");
    ESP_RETURN_ON_ERROR(test_selftest_run(&sys), TAG, "test_selftest_run");
#endif

    ESP_RETURN_ON_ERROR(task_measure_start(), TAG, "task_measure_start");
    ESP_RETURN_ON_ERROR(task_radio_start(),   TAG, "task_radio_start");

    xEventGroupSetBits(eg_state, EG_STATE_SYSTEM_READY);
    sys.init_done = true;

    ESP_LOGI(TAG, "boot ok");
    return ESP_OK;
}
