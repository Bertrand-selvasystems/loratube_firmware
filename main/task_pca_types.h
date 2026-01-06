#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Pins PCA : 0..3
typedef uint8_t pca_pin_t;

typedef enum {
    PCA_CMD_SET_DIR = 0,
    PCA_CMD_SET_LEVEL,
    PCA_CMD_BLINK_START,
    PCA_CMD_BLINK_STOP,
} pca_cmd_op_t;

typedef struct {
    pca_cmd_op_t op;
    pca_pin_t pin;

    union {
        struct { bool is_output; } set_dir;
        struct { bool level; } set_level;
        struct { uint32_t on_ms; uint32_t off_ms; bool start_high; } blink;
    } u;

    // optionnel : retour synchrone (même pattern que I2C)
    // Si tu ne veux que de l’async, laisse NULL.
    TaskHandle_t reply_task;
    uint32_t     reply_tag;
    esp_err_t   *out_result;
} pca_cmd_t;
