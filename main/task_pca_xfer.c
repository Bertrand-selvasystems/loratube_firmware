#include "task_pca_xfer.h"
#include "task_i2c_types.h"   // i2c_cmd_t, i2c_op_t
#include "freertos/task.h"
#include "esp_err.h"

static esp_err_t post_and_wait(task_pca_i2c_xfer_ctx_t *c, i2c_cmd_t *cmd)
{
    if (!c || !cmd || !cmd->out_result) return ESP_ERR_INVALID_ARG;

    // Envoi commande à task_i2c
    if (xQueueSend(c->q_cmd, cmd, cmd->timeout_ticks) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    // Attente notification de fin (valeur = reply_tag)
    uint32_t v = 0;
    BaseType_t ok = xTaskNotifyWait(0, 0xFFFFFFFFu, &v, cmd->timeout_ticks);
    if (ok != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    // Filtrage minimal (évite les réveils parasites)
    if (v != cmd->reply_tag) {
        return ESP_ERR_INVALID_STATE;
    }

    return *(cmd->out_result);
}

static esp_err_t xfer_write_(void *ctx, uint8_t addr7,
                             const uint8_t *w, uint16_t wlen,
                             i2c_to_t to)
{
    task_pca_i2c_xfer_ctx_t *c = (task_pca_i2c_xfer_ctx_t*)ctx;
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
        .reply_tag  = ++c->next_tag,
        .out_result = &result,
    };

    return post_and_wait(c, &cmd);
}

static esp_err_t xfer_read_(void *ctx, uint8_t addr7,
                            uint8_t *r, uint16_t rlen,
                            i2c_to_t to)
{
    task_pca_i2c_xfer_ctx_t *c = (task_pca_i2c_xfer_ctx_t*)ctx;
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
        .reply_tag  = ++c->next_tag,
        .out_result = &result,
    };

    return post_and_wait(c, &cmd);
}

static esp_err_t xfer_write_read_(void *ctx, uint8_t addr7,
                                  const uint8_t *w, uint16_t wlen,
                                  uint8_t *r, uint16_t rlen,
                                  i2c_to_t to)
{
    task_pca_i2c_xfer_ctx_t *c = (task_pca_i2c_xfer_ctx_t*)ctx;
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
        .reply_tag  = ++c->next_tag,
        .out_result = &result,
    };

    return post_and_wait(c, &cmd);
}

void task_pca_xfer_init(task_pca_i2c_xfer_ctx_t *ctx,
                        QueueHandle_t q_i2c_cmd,
                        i2c_xfer_t *out_xfer)
{
    if (!ctx || !out_xfer || !q_i2c_cmd) return;

    ctx->q_cmd = q_i2c_cmd;
    ctx->next_tag = 0;

    *out_xfer = (i2c_xfer_t){
        .ctx = ctx,
        .write = xfer_write_,
        .read = xfer_read_,
        .write_read = xfer_write_read_,
    };
}
