#pragma once
/**
 * @file system_cfg.h
 * @brief Frozen hardware configuration (wiring + design constants). MUST be const and never modified.
 *
 * This is the "schematic in C form". If hardware changes, firmware changes.
 *
 * Allowed:
 *  - pins, ports, I2C addresses, bus frequencies, timeouts (design-level)
 *  - module-level immutable configs (const)
 *
 * Forbidden:
 *  - FreeRTOS types
 *  - queues/event bits
 *  - runtime flags ("enable_xxx", blink policy, etc.)
 *
 * Example:
 *  typedef struct {
 *    i2c_port_t i2c_port;
 *    gpio_num_t sda_gpio;
 *    gpio_num_t scl_gpio;
 *    uint32_t   i2c_freq_hz;
 *    uint8_t    some_i2c_addr;
 *  } system_cfg_t;
 *
 *  extern const system_cfg_t SYS_CFG;
 */

#include <stdint.h>
#include "driver/i2c.h"
#include "driver/gpio.h"

typedef struct {
    i2c_port_t i2c_port;
    gpio_num_t sda_gpio;
    gpio_num_t scl_gpio;
    uint32_t   i2c_freq_hz;
    uint32_t   i2c_timeout_ms;
} system_i2c_cfg_t;

typedef struct {
    system_i2c_cfg_t i2c;
    /* Add other immutable periphery configs here */
} system_cfg_t;

extern const system_cfg_t SYS_CFG;
