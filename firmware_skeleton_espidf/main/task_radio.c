/**
 * @file task_radio.c
 * @brief Radio task implementation.
 */

#include "task_radio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_check.h"

#include "system_config.h"
#include "system_types.h"
#include "system_queues.h"
#include "system_state.h"
#include "system_faults.h"

#include "module_dummy_radio.h"

static const char *TAG = "TASK_RADIO";

static TaskHandle_t s_task = NULL;
static dummy_radio_t s_radio;

static void task_fn(void *arg)
{
    (void)arg;

    for (;;) {
        radio_cmd_t cmd = {0};

        if (!q_radio_cmd) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // FIFO: wait for commands
        if (xQueueReceive(q_radio_cmd, &cmd, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        esp_err_t err = ESP_OK;

        switch (cmd.kind) {
            case RADIO_CMD_SEND_PING: {
                const uint8_t payload[4] = { 'P','I','N','G' };
                err = dummy_radio_send(&s_radio, payload, sizeof(payload));
            } break;
            case RADIO_CMD_SEND_TEXT: {
                err = dummy_radio_send(&s_radio, cmd.text, strnlen(cmd.text, sizeof(cmd.text)));
            } break;
            default:
                // nop
                break;
        }

        if (err != ESP_OK) {
            system_faults_set(FAULT_DRIVER_RADIO);
            ESP_LOGE(TAG, "radio cmd failed: %s", esp_err_to_name(err));
        } else {
            system_faults_clear(FAULT_DRIVER_RADIO);
            if (eg_state) xEventGroupSetBits(eg_state, EG_STATE_RADIO_TX_DONE);
        }
    }
}

esp_err_t task_radio_start(void)
{
    if (s_task) return ESP_ERR_INVALID_STATE;
    if (!q_radio_cmd || !eg_state) return ESP_ERR_INVALID_STATE;

    dummy_radio_cfg_t cfg = { .bitrate = 38400 };
    ESP_RETURN_ON_ERROR(dummy_radio_init(&s_radio, &cfg), TAG, "dummy_radio_init");

    BaseType_t ok = xTaskCreate(task_fn, "task_radio", TASK_STACK_RADIO, NULL, TASK_PRIO_RADIO, &s_task);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
