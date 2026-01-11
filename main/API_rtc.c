// api_rtc.c
#include "API_rtc.h"
#include "task_rtc_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static QueueHandle_t s_q = NULL;

void api_rtc_bind_queue(QueueHandle_t q) 
{
    s_q = q;
}

static esp_err_t post_sync_(rtc_cmd_t *cmd, uint32_t to_ticks)
{
    if (!s_q || !cmd || !cmd->out_result) return ESP_ERR_INVALID_STATE;
    if (xQueueSend(s_q, cmd, to_ticks) != pdTRUE) return ESP_ERR_TIMEOUT;

    uint32_t v = 0;
    if (xTaskNotifyWait(0, 0xFFFFFFFFu, &v, to_ticks) != pdTRUE) return ESP_ERR_TIMEOUT;
    if (v != cmd->reply_tag) return ESP_ERR_INVALID_STATE;

    return *(cmd->out_result);
}

esp_err_t api_rtc_init_irq_timer(uint32_t to_ticks)
{
    static uint32_t tag = 0;
    esp_err_t res = ESP_FAIL;
    rtc_cmd_t cmd = {
        .op = RTC_CMD_INIT_IRQ_TIMER,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .reply_tag  = ++tag,
        .out_result = &res
    };
    return post_sync_(&cmd, to_ticks);
}

esp_err_t api_rtc_make_sqw_hi_z(uint32_t to_ticks)
{
    static uint32_t tag = 0;
    esp_err_t res = ESP_FAIL;
    rtc_cmd_t cmd = {
        .op = RTC_CMD_MAKE_SQW_HI_Z,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .reply_tag  = ++tag,
        .out_result = &res
    };
    return post_sync_(&cmd, to_ticks);
}

esp_err_t api_rtc_get_datetime(rtc_datetime_t *out_dt, uint32_t to_ticks)
{
    static uint32_t s_tag = 0;
    esp_err_t res = ESP_FAIL;

    rtc_cmd_t cmd = {
        .op = RTC_CMD_GET_DATETIME,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .reply_tag  = ++s_tag,
        .out_result = &res,
        .u.get_dt = { .out_dt = out_dt },
    };

    return post_sync_(&cmd, to_ticks);
}

esp_err_t api_rtc_set_datetime(const rtc_datetime_t *dt, uint32_t to_ticks)
{
    static uint32_t s_tag = 0;
    esp_err_t res = ESP_FAIL;

    rtc_cmd_t cmd = {
        .op = RTC_CMD_SET_DATETIME,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .reply_tag  = ++s_tag,
        .out_result = &res,
        .u.set_dt = { .dt = dt },
    };

    return post_sync_(&cmd, to_ticks);
}

// esp_err_t api_rtc_read_reg(uint8_t reg, uint8_t *out_val, uint32_t to_ticks)
// {
//     static uint32_t s_tag = 0;
//     esp_err_t res = ESP_FAIL;

//     rtc_cmd_t cmd = {
//         .op = RTC_CMD_READ_REG,
//         .reply_task = xTaskGetCurrentTaskHandle(),
//         .reply_tag  = ++s_tag,
//         .out_result = &res,
//         .u.read_reg = { .reg = reg, .out_val = out_val },
//     };

//     return post_sync_(&cmd, to_ticks);
// }

// esp_err_t api_rtc_write_reg(uint8_t reg, uint8_t val, uint32_t to_ticks)
// {
//     static uint32_t s_tag = 0;
//     esp_err_t res = ESP_FAIL;

//     rtc_cmd_t cmd = {
//         .op = RTC_CMD_WRITE_REG,
//         .reply_task = xTaskGetCurrentTaskHandle(),
//         .reply_tag  = ++s_tag,
//         .out_result = &res,
//         .u.write_reg = { .reg = reg, .val = val },
//     };

//     return post_sync_(&cmd, to_ticks);
// }
