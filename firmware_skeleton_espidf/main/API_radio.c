/**
 * @file API_radio.c
 * @brief Radio API implementation.
 */

#include "API_radio.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "system_config.h"
#include "system_types.h"
#include "system_queues.h"

static esp_err_t send_cmd(const radio_cmd_t *cmd)
{
    if (!q_radio_cmd) return ESP_ERR_INVALID_STATE;
    TickType_t t = pdMS_TO_TICKS(QUEUE_SEND_TIMEOUT_MS);
    return (xQueueSend(q_radio_cmd, cmd, t) == pdTRUE) ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t API_radio_send_ping(void)
{
    radio_cmd_t cmd = { .kind = RADIO_CMD_SEND_PING, .arg_u32 = 0 };
    return send_cmd(&cmd);
}

esp_err_t API_radio_send_text(const char *text)
{
    if (!text) return ESP_ERR_INVALID_ARG;
    radio_cmd_t cmd = { .kind = RADIO_CMD_SEND_TEXT, .arg_u32 = 0 };
    strncpy(cmd.text, text, sizeof(cmd.text) - 1);
    cmd.text[sizeof(cmd.text) - 1] = '\0';
    return send_cmd(&cmd);
}
