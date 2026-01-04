#pragma once
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum {
    I2C_OP_WRITE = 0,
    I2C_OP_READ,
    I2C_OP_WRITE_READ
} i2c_op_t;

typedef struct {
    i2c_op_t op;
    uint8_t  addr7;

    const uint8_t *w;
    size_t wlen;

    uint8_t *r;
    size_t rlen;

    uint32_t timeout_ticks;

    // Completion routing (caller fills)
    TaskHandle_t reply_task;
    uint32_t     reply_tag;

    // Result storage (caller-owned, static in your policy)
    esp_err_t   *out_result;
} i2c_cmd_t;
