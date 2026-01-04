#include "task_i2c.h"
#include "task_i2c_types.h"
#include "system_queues.h"
#include "system_cfg.h"        // fournit g_i2c0_cfg (const)
#include "system_config.h"        // fournit g_i2c0_cfg (const)
#include "module_i2c_bus.h"          

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

static const char *TAG = "TASK_I2C";

static void task_i2c_main(void *arg)
{
    (void)arg;

    esp_err_t err = i2c_bus_init(&g_hw_cfg.g_i2c0_cfg);
    ESP_LOGI(TAG, "i2c init: %s", esp_err_to_name(err));

    i2c_cmd_t cmd;

    for (;;) {
        if (xQueueReceive(g_q_i2c_cmd, &cmd, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        esp_err_t op_err = ESP_FAIL;

        switch (cmd.op) {
        case I2C_OP_WRITE:
            op_err = i2c_bus_write(g_hw_cfg.g_i2c0_cfg.port, cmd.addr7, cmd.w, cmd.wlen, cmd.timeout_ticks);
            break;

        case I2C_OP_READ:
            op_err = i2c_bus_read(g_hw_cfg.g_i2c0_cfg.port, cmd.addr7, cmd.r, cmd.rlen, cmd.timeout_ticks);
            break;

        case I2C_OP_WRITE_READ:
            op_err = i2c_bus_write_read(g_hw_cfg.g_i2c0_cfg.port, cmd.addr7, cmd.w, cmd.wlen, cmd.r, cmd.rlen, cmd.timeout_ticks);
            break;

        default:
            op_err = ESP_ERR_INVALID_ARG;
            break;
        }

        if (cmd.out_result) {
            *cmd.out_result = op_err;
        }

        if (cmd.reply_task) {
            xTaskNotify(cmd.reply_task, cmd.reply_tag, eSetValueWithOverwrite);
        }
    }
}

void task_i2c_start(void)
{
    xTaskCreate(task_i2c_main, "task_i2c",
                SYS_TASK_I2C_STACK, NULL,
                SYS_TASK_I2C_PRIO, NULL);
}
