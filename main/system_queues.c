/**
 * @file system_queues.c
 * @brief Implementation of global queues.
 */

#include "system_queues.h"
#include "system_config.h"
#include "esp_log.h"
#include "system_config.h"


static const char *TAG = "SYS_QUEUES";

QueueHandle_t g_q_log = NULL;

esp_err_t system_queues_init(void)
{
    if (g_q_log) return ESP_ERR_INVALID_STATE;

    g_q_log = xQueueCreate(SYS_Q_LOG_DEPTH, sizeof(log_msg_t));
    if (!g_q_log) goto oom;

    system_queues_dump();
    return ESP_OK;

oom:
    system_queues_deinit();
    return ESP_ERR_NO_MEM;
}

void system_queues_deinit(void)
{
    if (g_q_log) { vQueueDelete(g_q_log); g_q_log = NULL; }
}

void system_queues_dump(void)
{
    if (!g_q_log) {
        ESP_LOGW(TAG, "g_q_log=NULL");
        return;
    }

    const UBaseType_t waiting = uxQueueMessagesWaiting(g_q_log);
    const UBaseType_t spaces  = uxQueueSpacesAvailable(g_q_log);

    ESP_LOGI(TAG,
             "g_q_log=%p item=%u depth=%u waiting=%u free=%u",
             (void*)g_q_log,
             (unsigned)sizeof(log_msg_t),
             (unsigned)SYS_Q_LOG_DEPTH,
             (unsigned)waiting,
             (unsigned)spaces);
}
