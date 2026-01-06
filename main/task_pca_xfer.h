#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "xfer_i2c.h"

typedef struct {
    QueueHandle_t q_cmd;   // vers task_i2c
    uint32_t next_tag;     // tag monotonique pour filtrer la notif
} task_pca_i2c_xfer_ctx_t;

// Initialise ctx + remplit un i2c_xfer_t injectable dans module_pca9536
void task_pca_xfer_init(task_pca_i2c_xfer_ctx_t *ctx,
                        QueueHandle_t q_i2c_cmd,
                        i2c_xfer_t *out_xfer);
