#pragma once
#include <stdint.h>
#include "esp_err.h"

// Timeout en ticks FreeRTOS côté tasks.
// Ici on reste neutre : juste un typedef.
typedef uint32_t i2c_to_t;

typedef struct {
    void *ctx;

    esp_err_t (*write)(void *ctx, uint8_t addr7,
                       const uint8_t *w, uint16_t wlen,
                       i2c_to_t to);

    esp_err_t (*read)(void *ctx, uint8_t addr7,
                      uint8_t *r, uint16_t rlen,
                      i2c_to_t to);

    esp_err_t (*write_read)(void *ctx, uint8_t addr7,
                            const uint8_t *w, uint16_t wlen,
                            uint8_t *r, uint16_t rlen,
                            i2c_to_t to);
} i2c_xfer_t;