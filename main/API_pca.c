#include "api_pca.h"
#include "task_pca_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static QueueHandle_t s_q_pca = NULL;

void api_pca_bind_queue(void *q_pca_cmd)
{
    s_q_pca = (QueueHandle_t)q_pca_cmd;
}

static esp_err_t post_cmd_async(const pca_cmd_t *cmd, uint32_t to_ticks)
{
    if (!s_q_pca || !cmd) return ESP_ERR_INVALID_STATE;
    if (xQueueSend(s_q_pca, cmd, to_ticks) != pdTRUE) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

static esp_err_t post_cmd_sync(pca_cmd_t *cmd, uint32_t to_ticks)
{
    if (!s_q_pca || !cmd || !cmd->out_result) return ESP_ERR_INVALID_STATE;

    // Envoi
    if (xQueueSend(s_q_pca, cmd, to_ticks) != pdTRUE) return ESP_ERR_TIMEOUT;

    // Attente notif (valeur=reply_tag)
    uint32_t v = 0;
    BaseType_t ok = xTaskNotifyWait(0, 0xFFFFFFFFu, &v, to_ticks);
    if (ok != pdTRUE) return ESP_ERR_TIMEOUT;
    if (v != cmd->reply_tag) return ESP_ERR_INVALID_STATE;

    return *(cmd->out_result);
}

esp_err_t api_pca_set_dir_async(uint8_t pin, bool is_output, uint32_t to_ticks)
{
    pca_cmd_t cmd = {
        .op = PCA_CMD_SET_DIR,
        .pin = pin,
        .u.set_dir = { .is_output = is_output },
        .reply_task = NULL,
        .reply_tag = 0,
        .out_result = NULL,
    };
    return post_cmd_async(&cmd, to_ticks);
}

esp_err_t api_pca_set_level_async(uint8_t pin, bool level, uint32_t to_ticks)
{
    pca_cmd_t cmd = {
        .op = PCA_CMD_SET_LEVEL,
        .pin = pin,
        .u.set_level = { .level = level },
        .reply_task = NULL,
        .reply_tag = 0,
        .out_result = NULL,
    };
    return post_cmd_async(&cmd, to_ticks);
}

esp_err_t api_pca_blink_start_async(uint8_t pin, uint32_t on_ms, uint32_t off_ms, bool start_high, uint32_t to_ticks)
{
    pca_cmd_t cmd = {
        .op = PCA_CMD_BLINK_START,
        .pin = pin,
        .u.blink = { .on_ms = on_ms, .off_ms = off_ms, .start_high = start_high },
        .reply_task = NULL,
        .reply_tag = 0,
        .out_result = NULL,
    };
    return post_cmd_async(&cmd, to_ticks);
}

esp_err_t api_pca_blink_stop_async(uint8_t pin, uint32_t to_ticks)
{
    pca_cmd_t cmd = {
        .op = PCA_CMD_BLINK_STOP,
        .pin = pin,
        .reply_task = NULL,
        .reply_tag = 0,
        .out_result = NULL,
    };
    return post_cmd_async(&cmd, to_ticks);
}

// Exemple sync : set_level()
esp_err_t api_pca_set_level(uint8_t pin, bool level, uint32_t to_ticks)
{
    static uint32_t s_tag = 0;
    esp_err_t res = ESP_FAIL;

    pca_cmd_t cmd = {
        .op = PCA_CMD_SET_LEVEL,
        .pin = pin,
        .u.set_level = { .level = level },
        .reply_task = xTaskGetCurrentTaskHandle(),
        .reply_tag = ++s_tag,
        .out_result = &res,
    };

    return post_cmd_sync(&cmd, to_ticks);
}
