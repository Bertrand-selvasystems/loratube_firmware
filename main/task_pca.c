#include "task_pca.h"
#include "task_pca_types.h"
#include "task_pca_xfer.h"

#include "module_pca9536.h"   // ton module pur
#include "system_queues.h"    // g_q_i2c_cmd

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "TASK_PCA";

#ifndef PCA_TASK_STACK
#define PCA_TASK_STACK 4096
#endif

#ifndef PCA_TASK_PRIO
#define PCA_TASK_PRIO  10
#endif

#ifndef PCA_Q_DEPTH
#define PCA_Q_DEPTH 16
#endif

typedef struct {
    bool enabled;
    uint32_t on_ticks;
    uint32_t off_ticks;
    bool level;
    TickType_t next_toggle;
} blink_state_t;

static QueueHandle_t s_q_pca = NULL;

static task_pca_i2c_xfer_ctx_t s_xfer_ctx;
static i2c_xfer_t              s_xfer;

static pca9536_t               s_pca;
static uint8_t                 s_addr7 = 0x00;

static blink_state_t s_blink[4];

void *task_pca_get_queue(void)
{
    return (void*)s_q_pca;
}

static inline bool pin_ok(uint8_t pin) { return pin < 4; }

static void blink_tick(i2c_to_t to)
{
    TickType_t now = xTaskGetTickCount();
    for (uint8_t pin = 0; pin < 4; pin++) {
        blink_state_t *b = &s_blink[pin];
        if (!b->enabled) continue;

        // time to toggle?
        if ((int32_t)(now - b->next_toggle) >= 0) {
            // flip
            b->level = !b->level;

            // apply to PCA (sync)
            (void)pca9536_set_pin_level(&s_pca, (pca9536_pin_t)pin, b->level, to, true);

            // schedule next
            TickType_t dt = b->level ? b->on_ticks : b->off_ticks;
            if (dt == 0) dt = 1;
            b->next_toggle = now + dt;
        }
    }
}

static void reply_if_needed(const pca_cmd_t *cmd, esp_err_t res)
{
    if (cmd->out_result) *(cmd->out_result) = res;
    if (cmd->reply_task) xTaskNotify(cmd->reply_task, cmd->reply_tag, eSetValueWithOverwrite);
}

static void task_pca_main(void *arg)
{
    (void)arg;

    // // Queue commandes PCA
    // s_q_pca = xQueueCreate(PCA_Q_DEPTH, sizeof(pca_cmd_t));
    // if (!s_q_pca) {
    //     ESP_LOGE(TAG, "Failed to create PCA queue");
    //     vTaskDelete(NULL);
    //     return;
    // }

    // XFER vers task_i2c
    task_pca_xfer_init(&s_xfer_ctx, g_q_i2c_cmd, &s_xfer);

    // Init module PCA
    const i2c_to_t init_to = pdMS_TO_TICKS(250);
    esp_err_t err = pca9536_init(&s_pca, &s_xfer, s_addr7, init_to);
    ESP_LOGI(TAG, "pca init: %s", esp_err_to_name(err));

    // Option : mettre toutes les pins en output au boot (à toi de décider)
    // pca9536_write_reg(&s_pca, 0x03, 0x00, init_to);

    // Loop
    const TickType_t tick = pdMS_TO_TICKS(10); // résolution blink
    TickType_t last = xTaskGetTickCount();

    for (;;) {
        pca_cmd_t cmd;

        // Poll queue avec timeout court pour blink
        if (xQueueReceive(s_q_pca, &cmd, tick) == pdTRUE) {
            esp_err_t res = ESP_OK;
            const i2c_to_t to = pdMS_TO_TICKS(200);

            if (!pin_ok(cmd.pin)) {
                res = ESP_ERR_INVALID_ARG;
                reply_if_needed(&cmd, res);
                continue;
            }

            switch (cmd.op) {
            case PCA_CMD_SET_DIR:
                res = pca9536_set_pin_dir(&s_pca, (pca9536_pin_t)cmd.pin, cmd.u.set_dir.is_output, to, true);
                reply_if_needed(&cmd, res);
                break;

            case PCA_CMD_SET_LEVEL:
                // Si blink actif, on peut décider que SET_LEVEL stoppe le blink (choix d’archi)
                s_blink[cmd.pin].enabled = false;
                res = pca9536_set_pin_level(&s_pca, (pca9536_pin_t)cmd.pin, cmd.u.set_level.level, to, true);
                reply_if_needed(&cmd, res);
                break;

            case PCA_CMD_BLINK_START: {
                blink_state_t *b = &s_blink[cmd.pin];
                b->enabled = true;
                b->on_ticks  = pdMS_TO_TICKS(cmd.u.blink.on_ms);
                b->off_ticks = pdMS_TO_TICKS(cmd.u.blink.off_ms);
                b->level = cmd.u.blink.start_high;

                // force niveau initial immédiatement
                res = pca9536_set_pin_level(&s_pca, (pca9536_pin_t)cmd.pin, b->level, to, true);

                TickType_t now = xTaskGetTickCount();
                TickType_t dt = b->level ? b->on_ticks : b->off_ticks;
                if (dt == 0) dt = 1;
                b->next_toggle = now + dt;

                reply_if_needed(&cmd, res);
                break;
            }

            case PCA_CMD_BLINK_STOP:
                s_blink[cmd.pin].enabled = false;
                reply_if_needed(&cmd, ESP_OK);
                break;

            default:
                reply_if_needed(&cmd, ESP_ERR_INVALID_ARG);
                break;
            }
        }

        // Blink engine tick
        blink_tick(pdMS_TO_TICKS(200));

        // Optionnel : vTaskDelayUntil pour cadence stable
        vTaskDelayUntil(&last, tick);
    }
}

void task_pca_start(uint8_t pca_addr7)
{
    s_addr7 = pca_addr7;

    if (!s_q_pca) {
        s_q_pca = xQueueCreate(PCA_Q_DEPTH, sizeof(pca_cmd_t));
    }

    xTaskCreate(task_pca_main, "task_pca",
                PCA_TASK_STACK, NULL,
                PCA_TASK_PRIO, NULL);
}
