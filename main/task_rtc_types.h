#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtc_types.h"   

typedef enum {
    RTC_CMD_INIT_IRQ_TIMER = 0,
    RTC_CMD_MAKE_SQW_HI_Z,
    RTC_CMD_GET_DATETIME,
    RTC_CMD_SET_DATETIME,
    RTC_CMD_READ_REG,
    RTC_CMD_WRITE_REG,
} rtc_cmd_op_t;

typedef struct {
    rtc_cmd_op_t op;

    // Timeout de l’opération (ticks). La task peut choisir d’ignorer et utiliser un défaut.
    uint32_t timeout_ticks;

    // mécanisme sync optionnel (comme ton PCA / I2C)
    TaskHandle_t reply_task;
    uint32_t     reply_tag;
    esp_err_t   *out_result;

    union {
        struct { rtc_datetime_t *out_dt; } get_dt;
        struct { const rtc_datetime_t *dt; } set_dt;

        struct { uint8_t reg; uint8_t *out_val; } read_reg;
        struct { uint8_t reg; uint8_t val; } write_reg;
    } u;

} rtc_cmd_t;
