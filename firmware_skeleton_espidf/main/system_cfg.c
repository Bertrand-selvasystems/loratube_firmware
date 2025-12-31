/**
 * @file system_cfg.c
 * @brief Definition of the frozen hardware configuration.
 *
 * Note: GPIO numbers here are placeholders for the skeleton.
 * Replace with your actual wiring.
 */

#include "system_cfg.h"

const system_cfg_t SYS_CFG = {
    .i2c = {
        .i2c_port = I2C_NUM_0,
        .sda_gpio = GPIO_NUM_8,
        .scl_gpio = GPIO_NUM_9,
        .i2c_freq_hz = 100000,
        .i2c_timeout_ms = 50,
    },
};
