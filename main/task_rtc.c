#include "task_rtc.h"
#include "task_rtc_types.h"
#include "module_pcf8523.h"

#include "system_queues.h"   // g_q_i2c_cmd, g_q_rtc_cmd
#include "xfer_i2c.h"        // i2c_xfer_t, i2c_to_t (ou xfer_i2c.h inclut xfer_i2c.h -> à adapter)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "TASK_RTC";

#ifndef SYS_TASK_RTC_STACK
#define SYS_TASK_RTC_STACK 3072
#endif

#ifndef SYS_TASK_RTC_PRIO
#define SYS_TASK_RTC_PRIO  8
#endif

#ifndef RTC_BUF_SZ
#define RTC_BUF_SZ 32
#endif

// --- state task ---
static uint8_t s_addr7 = 0;

// bridge vers task_i2c
static i2c_xfer_t     s_xfer;

// buffers locaux (pas d’allocation)
static uint8_t s_wbuf[RTC_BUF_SZ];
static uint8_t s_rbuf[RTC_BUF_SZ];

//static QueueHandle_t s_q_rtc = NULL;

// ------------------ helpers ------------------

static void reply_(const rtc_cmd_t *cmd, esp_err_t res)
{
    if (cmd->out_result) *(cmd->out_result) = res;
    if (cmd->reply_task) xTaskNotify(cmd->reply_task, cmd->reply_tag, eSetValueWithOverwrite);
}

static inline i2c_to_t pick_to_(const rtc_cmd_t *cmd, i2c_to_t def_to)
{
    if (!cmd) return def_to;
    // si timeout_ticks=0, on prend un défaut
    return (cmd->timeout_ticks != 0) ? (i2c_to_t)cmd->timeout_ticks : def_to;
}

static esp_err_t rtc_read_reg_(uint8_t reg, uint8_t *out, i2c_to_t to)
{
    if (!out) return ESP_ERR_INVALID_ARG;

    s_wbuf[0] = reg;
    esp_err_t e = s_xfer.write_read(s_xfer.ctx, s_addr7, s_wbuf, 1, s_rbuf, 1, to);
    if (e == ESP_OK) *out = s_rbuf[0];
    return e;
}

static esp_err_t rtc_write_reg_(uint8_t reg, uint8_t val, i2c_to_t to)
{
    s_wbuf[0] = reg;
    s_wbuf[1] = val;
    return s_xfer.write(s_xfer.ctx, s_addr7, s_wbuf, 2, to);
}

// --- Séquence 1 : IRQ timer open-drain pulse + CLKOUT off ---
static esp_err_t rtc_init_irq_timer_(i2c_to_t to)
{
    esp_err_t e;
    uint8_t v0f = 0, v01 = 0;

    e = rtc_read_reg_(PCF8523_REG_TMR_CLKOUT, &v0f, to);
    if (e != ESP_OK) { ESP_LOGE(TAG, "NACK @0x0F: %s", esp_err_to_name(e)); return e; }
    ESP_LOGW(TAG, "0x0F initial = 0x%02X", v0f);

    e = rtc_read_reg_(PCF8523_REG_CTRL2, &v01, to);
    if (e != ESP_OK) { ESP_LOGE(TAG, "NACK @0x01: %s", esp_err_to_name(e)); return e; }
    ESP_LOGW(TAG, "0x01 initial = 0x%02X", v01);

    // 0x0F : CLKOUT off + timers off (valeur figée côté module)
    e = rtc_write_reg_(PCF8523_REG_TMR_CLKOUT, PCF8523_TMRCLKOUT_VAL_OFF, to);
    if (e != ESP_OK) return e;

    // 0x01 : TI_TP=1 (pulse) + TIE=1, tout le reste à 0, flags cleared
    uint8_t ctrl2 = (uint8_t)(PCF8523_CTRL2_TI_TP | PCF8523_CTRL2_TIE);
    e = rtc_write_reg_(PCF8523_REG_CTRL2, ctrl2, to);
    if (e != ESP_OK) return e;

    // readback
    e = rtc_read_reg_(PCF8523_REG_TMR_CLKOUT, &v0f, to);
    if (e != ESP_OK) return e;
    ESP_LOGI(TAG, "0x0F after = 0x%02X (expect 0x38)", v0f);

    e = rtc_read_reg_(PCF8523_REG_CTRL2, &v01, to);
    if (e != ESP_OK) return e;
    ESP_LOGI(TAG, "0x01 after = 0x%02X (expect TI_TP=1,TIE=1)", v01);

    return ESP_OK;
}

// --- Séquence 2 : SQW Hi-Z (CLKOUT off + IRQ off) ---
static esp_err_t rtc_make_sqw_hi_z_(i2c_to_t to)
{
    esp_err_t e;

    e = rtc_write_reg_(PCF8523_REG_TMR_CLKOUT, PCF8523_TMRCLKOUT_VAL_OFF, to);
    if (e != ESP_OK) return e;

    e = rtc_write_reg_(PCF8523_REG_CTRL2, 0x00, to);
    if (e != ESP_OK) return e;

    uint8_t v;
    e = rtc_read_reg_(PCF8523_REG_TMR_CLKOUT, &v, to);
    if (e != ESP_OK) return e;
    ESP_LOGI(TAG, "0x0F -> 0x%02X (expect 0x38)", v);

    e = rtc_read_reg_(PCF8523_REG_CTRL2, &v, to);
    if (e != ESP_OK) return e;
    ESP_LOGI(TAG, "0x01 -> 0x%02X (expect 0x00)", v);

    return ESP_OK;
}

static esp_err_t rtc_get_datetime_(rtc_datetime_t *out, i2c_to_t to)
{
    if (!out) return ESP_ERR_INVALID_ARG;

    // Lecture 7 regs secs..year (selon ton module)
    s_wbuf[0] = PCF8523_REG_SECONDS;
    esp_err_t e = s_xfer.write_read(s_xfer.ctx, s_addr7, s_wbuf, 1, s_rbuf, 7, to);
    if (e != ESP_OK) return e;

    return pcf8523_decode_datetime(out, s_rbuf);
}

static esp_err_t rtc_set_datetime_(const rtc_datetime_t *dt, i2c_to_t to)
{
    if (!dt) return ESP_ERR_INVALID_ARG;

    uint8_t payload[7];
    esp_err_t e = pcf8523_encode_datetime(payload, dt);
    if (e != ESP_OK) return e;

    s_wbuf[0] = PCF8523_REG_SECONDS;
    for (int i = 0; i < 7; i++) s_wbuf[1 + i] = payload[i];

    return s_xfer.write(s_xfer.ctx, s_addr7, s_wbuf, 8, to);
}

// ------------------ main task ------------------

static void task_rtc_main(void *arg)
{
    (void)arg;

    // préconditions
    if (!g_q_i2c_cmd) {
        ESP_LOGE(TAG, "g_q_i2c_cmd NULL (I2C bus task not ready)");
        vTaskDelete(NULL);
        return;
    }
    if (!g_q_rtc_cmd) {
        ESP_LOGE(TAG, "g_q_rtc_cmd NULL (RTC queue not created)");
        vTaskDelete(NULL);
        return;
    }



    const i2c_to_t def_to = pdMS_TO_TICKS(120);

    ESP_LOGI(TAG, "RTC task started addr7=0x%02X", s_addr7);

    for (;;) {
        rtc_cmd_t cmd;
        if (xQueueReceive(g_q_rtc_cmd, &cmd, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        i2c_to_t to = pick_to_(&cmd, def_to);
        esp_err_t res = ESP_OK;

        switch (cmd.op) {
        case RTC_CMD_INIT_IRQ_TIMER:
            res = rtc_init_irq_timer_(to);
            break;

        case RTC_CMD_MAKE_SQW_HI_Z:
            res = rtc_make_sqw_hi_z_(to);
            break;

        case RTC_CMD_GET_DATETIME:
            res = rtc_get_datetime_(cmd.u.get_dt.out_dt, to);
            break;

        case RTC_CMD_SET_DATETIME:
            res = rtc_set_datetime_(cmd.u.set_dt.dt, to);
            break;

        case RTC_CMD_READ_REG:
            res = rtc_read_reg_(cmd.u.read_reg.reg, cmd.u.read_reg.out_val, to);
            break;

        case RTC_CMD_WRITE_REG:
            res = rtc_write_reg_(cmd.u.write_reg.reg, cmd.u.write_reg.val, to);
            break;

        default:
            res = ESP_ERR_INVALID_ARG;
            break;
        }

        reply_(&cmd, res);
    }
}

void task_rtc_start(uint8_t addr7)
{
    s_addr7 = addr7;

    xTaskCreate(task_rtc_main,
                "task_rtc",
                SYS_TASK_RTC_STACK,
                NULL,
                SYS_TASK_RTC_PRIO,
                NULL);
}

void *task_rtc_get_queue(void)
{
    return (void*)g_q_rtc_cmd;
}

