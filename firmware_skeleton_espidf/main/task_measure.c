/**
 * @file task_measure.c
 * @brief Measure task implementation.
 */

#include "task_measure.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_check.h"
#include "esp_timer.h"

#include "system_config.h"
#include "system_types.h"
#include "system_queues.h"
#include "system_state.h"
#include "system_faults.h"

#include "module_dummy_sensor.h"

static const char *TAG = "TASK_MEASURE";

static TaskHandle_t s_task = NULL;
static dummy_sensor_t s_sensor;

static void task_fn(void *arg)
{
    (void)arg;

    for (;;) {
        float v = 0.0f;
        esp_err_t err = dummy_sensor_read(&s_sensor, &v);
        if (err != ESP_OK) {
            system_faults_set(FAULT_DRIVER_SENSOR);
            ESP_LOGE(TAG, "dummy_sensor_read failed: %s", esp_err_to_name(err));
        } else {
            measure_msg_t msg = {
                .value = v,
                .t_ms = (uint32_t)(esp_timer_get_time() / 1000ull),
            };

            // Depth=1 queue: overwrite keeps latest value only
            if (q_measure_latest) {
                xQueueOverwrite(q_measure_latest, &msg);
            }
            if (eg_state) {
                xEventGroupSetBits(eg_state, EG_STATE_MEASURE_DONE);
            }
            system_faults_clear(FAULT_DRIVER_SENSOR);
            ESP_LOGI(TAG, "measure=%.3f", (double)v);
        }

        vTaskDelay(pdMS_TO_TICKS(MEASURE_PERIOD_MS));
    }
}

esp_err_t task_measure_start(void)
{
    if (s_task) return ESP_ERR_INVALID_STATE;
    if (!q_measure_latest || !eg_state) return ESP_ERR_INVALID_STATE;

    dummy_sensor_cfg_t cfg = {
        .slope = 0.001f,
        .offset = 1.0f,
    };
    ESP_RETURN_ON_ERROR(dummy_sensor_init(&s_sensor, &cfg), TAG, "dummy_sensor_init");

    BaseType_t ok = xTaskCreate(task_fn, "task_measure", TASK_STACK_MEASURE, NULL, TASK_PRIO_MEASURE, &s_task);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
