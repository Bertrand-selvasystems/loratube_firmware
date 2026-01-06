/**
 * @file system_queues.c
 * @brief Implementation of global queues.
 */

#include "system_queues.h"
#include "system_config.h"
#include "esp_log.h"
#include "system_config.h"
#include "task_i2c_types.h"
#include "task_pca_types.h"

static const char *TAG = "SYS_QUEUES";

QueueHandle_t g_q_i2c_cmd;
QueueHandle_t g_q_log = NULL;
QueueHandle_t g_q_pca_cmd;


esp_err_t system_queues_init(void)
{
    if (g_q_log) return ESP_ERR_INVALID_STATE;
    g_q_log = xQueueCreate(SYS_Q_LOG_DEPTH, sizeof(log_msg_t));
    if (!g_q_log) goto oom;

    if (g_q_i2c_cmd) return ESP_ERR_INVALID_STATE;
    g_q_i2c_cmd = xQueueCreate(SYS_Q_I2C_CMD_DEPTH, sizeof(i2c_cmd_t));
    if (!g_q_i2c_cmd) goto oom;

    if (g_q_pca_cmd) return ESP_ERR_INVALID_STATE;
    g_q_pca_cmd = xQueueCreate(SYS_Q_PCA_CMD_DEPTH, sizeof(pca_cmd_t));
    if (!g_q_pca_cmd) goto oom;

    system_queues_dump();
    return ESP_OK;

oom:
    system_queues_deinit();
    return ESP_ERR_NO_MEM;
}

void system_queues_deinit(void)
{
    if (g_q_log) { vQueueDelete(g_q_log); g_q_log = NULL; }
    if (g_q_i2c_cmd) { vQueueDelete(g_q_i2c_cmd); g_q_i2c_cmd = NULL; }
}

void system_queues_dump(void)
{
    if (!g_q_log) {
        ESP_LOGW(TAG, "g_q_log=NULL");
        return;
    }
    if (!g_q_i2c_cmd) {
        ESP_LOGW(TAG, "g_q_i2c_cmd=NULL");
        return;
    }
    if (!g_q_pca_cmd) {
        ESP_LOGW(TAG, "g_q_pca_cmd=NULL");
        return;
    }

    ESP_LOGI(TAG,
             "g_q_log=%p item=%u depth=%u waiting=%u free=%u",
             (void*)g_q_log,
             (unsigned)sizeof(log_msg_t),
             (unsigned)SYS_Q_LOG_DEPTH,
             (unsigned)uxQueueMessagesWaiting(g_q_log),
             (unsigned)uxQueueSpacesAvailable(g_q_log));

        ESP_LOGI(TAG, "g_q_i2c_cmd=%p item=%u depth=%u waiting=%u free=%u",
             (void*)g_q_i2c_cmd,
             (unsigned)sizeof(i2c_cmd_t),
             (unsigned)SYS_Q_I2C_CMD_DEPTH,
             (unsigned)uxQueueMessagesWaiting(g_q_i2c_cmd),
             (unsigned)uxQueueSpacesAvailable(g_q_i2c_cmd));

        ESP_LOGI(TAG, "g_q_pca_cmd=%p item=%u depth=%u waiting=%u free=%u",
             (void*)g_q_pca_cmd,
             (unsigned)sizeof(pca_cmd_t),
             (unsigned)SYS_Q_PCA_CMD_DEPTH,
             (unsigned)uxQueueMessagesWaiting(g_q_pca_cmd),
             (unsigned)uxQueueSpacesAvailable(g_q_pca_cmd));

}
