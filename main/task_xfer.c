#include "task_xfer.h"
#include "task_i2c_types.h"   // i2c_cmd_t, i2c_op_t
#include "freertos/task.h"
#include "esp_err.h"

/* Pour bien clarifier, il y a 3 couches

A) Interface (vtable)
i2c_xfer_t contient des pointeurs :
.write = xfer_write_
.read = xfer_read_
.write_read = xfer_write_read_
C’est l’API que les modules utilisent.
Tous les appels des modules passent par là.

B) Proxy côté client (ce fichier)
Les fonctions xfer_* :
construisent un i2c_cmd_t
l’envoient à l’owner via xQueueSend
attendent la fin via ulTaskNotifyTake
renvoient *out_result
C’est l’adaptateur entre “appel synchrone” et “owner asynchrone”.

C) Owner I²C (dans task_i2c.c)
L’owner reçoit i2c_cmd_t dans un while, 
exécute la transaction matérielle, 
écrit *out_result, 
et fait xTaskNotifyGive(reply_task).
*/


static void drain_notif(void)
{
    while (ulTaskNotifyTake(pdTRUE, 0) > 0) { }
}

static esp_err_t post_and_wait(task_i2c_xfer_ctx_t *c, i2c_cmd_t *cmd)
{
    if (!c || !cmd || !cmd->out_result) return ESP_ERR_INVALID_ARG;

    drain_notif(); // évite de consommer un vieux "give"

    if (xQueueSend(c->q_cmd, cmd, cmd->timeout_ticks) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    if (ulTaskNotifyTake(pdTRUE, cmd->timeout_ticks) == 0) {
        return ESP_ERR_TIMEOUT;
    }

    return *(cmd->out_result);
}



static esp_err_t xfer_write_(void *ctx, uint8_t addr7,
                             const uint8_t *w, uint16_t wlen,
                             i2c_to_t to)
{
    task_i2c_xfer_ctx_t *c = (task_i2c_xfer_ctx_t*)ctx;
    if (!c || !w || wlen == 0) return ESP_ERR_INVALID_ARG;

    esp_err_t result = ESP_FAIL;

    i2c_cmd_t cmd = {
        .op = I2C_OP_WRITE,
        .addr7 = addr7,
        .w = w,
        .wlen = (size_t)wlen,
        .r = NULL,
        .rlen = 0,
        .timeout_ticks = (uint32_t)to,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .out_result = &result,
    };

    return post_and_wait(c, &cmd);
}

static esp_err_t xfer_read_(void *ctx, uint8_t addr7,
                            uint8_t *r, uint16_t rlen,
                            i2c_to_t to)
{
    task_i2c_xfer_ctx_t *c = (task_i2c_xfer_ctx_t*)ctx;
    if (!c || !r || rlen == 0) return ESP_ERR_INVALID_ARG;

    esp_err_t result = ESP_FAIL;

    i2c_cmd_t cmd = {
        .op = I2C_OP_READ,
        .addr7 = addr7,
        .w = NULL,
        .wlen = 0,
        .r = r,
        .rlen = (size_t)rlen,
        .timeout_ticks = (uint32_t)to,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .out_result = &result,
    };

    return post_and_wait(c, &cmd);
}

static esp_err_t xfer_write_read_(void *ctx, uint8_t addr7,
                                  const uint8_t *w, uint16_t wlen,
                                  uint8_t *r, uint16_t rlen,
                                  i2c_to_t to)
{
    task_i2c_xfer_ctx_t *c = (task_i2c_xfer_ctx_t*)ctx;
    if (!c || !w || wlen == 0 || !r || rlen == 0) return ESP_ERR_INVALID_ARG;

    esp_err_t result = ESP_FAIL;

    i2c_cmd_t cmd = {
        .op = I2C_OP_WRITE_READ,
        .addr7 = addr7,
        .w = w,
        .wlen = (size_t)wlen,
        .r = r,
        .rlen = (size_t)rlen,
        .timeout_ticks = (uint32_t)to,
        .reply_task = xTaskGetCurrentTaskHandle(),
        .out_result = &result,
    };

    return post_and_wait(c, &cmd);
}

void task_xfer_init(task_i2c_xfer_ctx_t *ctx,
                        QueueHandle_t q_i2c_cmd,
                        i2c_xfer_t *out_xfer)
{
    if (!ctx || !out_xfer || !q_i2c_cmd) return;

    ctx->q_cmd = q_i2c_cmd;

    *out_xfer = (i2c_xfer_t){
        .ctx = ctx,
        .write = xfer_write_,
        .read = xfer_read_,
        .write_read = xfer_write_read_,
    };
}
