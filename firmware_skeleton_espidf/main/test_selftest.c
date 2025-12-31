/**
 * @file test_selftest.c
 * @brief DEV-only selftest implementation.
 */

#include "test_selftest.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "system_state.h"
#include "system_queues.h"
#include "system_faults.h"

#include "API_radio.h"

static const char *TAG = "SELFTEST";

esp_err_t test_selftest_run(system_t *sys)
{
    (void)sys;

    // Basic sanity
    if (!eg_state) {
        system_faults_set(FAULT_STATE_INIT);
        return ESP_FAIL;
    }
    if (!q_measure_latest || !q_radio_cmd) {
        system_faults_set(FAULT_QUEUE_INIT);
        return ESP_FAIL;
    }

    // Demonstrate overwrite queue pattern (depth=1)
    {
        measure_msg_t msg = { .value = 123.0f, .t_ms = 0 };
        xQueueOverwrite(q_measure_latest, &msg);
        msg.value = 456.0f;
        xQueueOverwrite(q_measure_latest, &msg);

        measure_msg_t out = {0};
        if (xQueueReceive(q_measure_latest, &out, 0) != pdTRUE) {
            system_faults_set(FAULT_SELFTEST_FAILED);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "overwrite queue keeps latest: value=%.1f (expected 456.0)", (double)out.value);
    }

    // Demonstrate FIFO queue pattern
    ESP_LOGI(TAG, "sending 2 radio commands into FIFO queue");
    ESP_ERROR_CHECK(API_radio_send_ping());
    ESP_ERROR_CHECK(API_radio_send_text("hello from selftest"));

    system_faults_clear(FAULT_SELFTEST_FAILED);
    xEventGroupSetBits(eg_state, EG_STATE_SELFTEST_OK);

    ESP_LOGI(TAG, "SELFTEST OK");
    return ESP_OK;
}
