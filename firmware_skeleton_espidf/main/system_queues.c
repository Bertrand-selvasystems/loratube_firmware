/**
 * @file system_queues.c
 * @brief Implementation of global queues.
 */

#include "system_queues.h"
#include "esp_log.h"
#include "system_config.h"

static const char *TAG = "SYS_QUEUES";

QueueHandle_t q_measure_latest = NULL;
QueueHandle_t q_radio_cmd = NULL;

esp_err_t system_queues_init(void)
{
    if (q_measure_latest || q_radio_cmd) return ESP_ERR_INVALID_STATE;

    q_measure_latest = xQueueCreate(1, sizeof(measure_msg_t));
    if (!q_measure_latest) goto oom;

    q_radio_cmd = xQueueCreate(RADIO_CMD_QUEUE_DEPTH, sizeof(radio_cmd_t));
    if (!q_radio_cmd) goto oom;

    system_queues_dump();
    return ESP_OK;

oom:
    system_queues_deinit();
    return ESP_ERR_NO_MEM;
}

void system_queues_deinit(void)
{
    if (q_measure_latest) { vQueueDelete(q_measure_latest); q_measure_latest = NULL; }
    if (q_radio_cmd)      { vQueueDelete(q_radio_cmd);      q_radio_cmd = NULL; }
}

void system_queues_dump(void)
{
    ESP_LOGI(TAG, "q_measure_latest=%p (item=%u depth=1 overwrite)",
             (void*)q_measure_latest, (unsigned)sizeof(measure_msg_t));
    ESP_LOGI(TAG, "q_radio_cmd=%p (item=%u depth=%u FIFO)",
             (void*)q_radio_cmd, (unsigned)sizeof(radio_cmd_t), (unsigned)RADIO_CMD_QUEUE_DEPTH);
    if (q_radio_cmd) {
        ESP_LOGI(TAG, "q_radio_cmd waiting=%u", (unsigned)uxQueueMessagesWaiting(q_radio_cmd));
    }
}
