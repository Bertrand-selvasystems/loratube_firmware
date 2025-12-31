#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t i2c_timeout_ticks_t; // opaque: c'est un nombre de ticks, fourni par les tasks

typedef struct {
    i2c_port_t port;
    gpio_num_t sda_gpio;
    gpio_num_t scl_gpio;
    uint32_t   clk_speed_hz;
    bool       enable_pullups;
} i2c_bus_cfg_t;

esp_err_t i2c_bus_init(const i2c_bus_cfg_t *cfg);
esp_err_t i2c_bus_deinit(i2c_port_t port);

esp_err_t i2c_bus_write(i2c_port_t port, uint8_t addr7,
                        const uint8_t *data, size_t len,
                        i2c_timeout_ticks_t timeout_ticks);

esp_err_t i2c_bus_read(i2c_port_t port, uint8_t addr7,
                       uint8_t *data, size_t len,
                       i2c_timeout_ticks_t timeout_ticks);

esp_err_t i2c_bus_write_read(i2c_port_t port, uint8_t addr7,
                            const uint8_t *w, size_t wlen,
                            uint8_t *r, size_t rlen,
                            i2c_timeout_ticks_t timeout_ticks);

#ifdef __cplusplus
}
#endif
