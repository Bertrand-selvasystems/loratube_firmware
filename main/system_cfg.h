#pragma once
/**
 * @file    system_cfg.h
 * @brief   Frozen hardware configuration (wiring, addresses, bus speeds).
 *
 * Rules:
 * - This is "frozen": const data, never modified at runtime.
 * - No FreeRTOS types here.
 * - OK to include ESP-IDF driver types (gpio_num_t, i2c_port_t) because this is hardware wiring.
 */

#include <stdint.h>
#include <stdbool.h>

#include "driver/gpio.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------
// I2C addresses defaults
// -------------------------
// PCA9536: base is 0x41 (A0/A1 fixed by wiring). Adjust to your board.
#ifndef CFG_PCA9536_ADDR7
#define CFG_PCA9536_ADDR7   (0x41u)
#endif

// PCF8523 RTC: 0x68
#ifndef CFG_PCF8523_ADDR7
#define CFG_PCF8523_ADDR7   (0x68u)
#endif

// MB85RC256V FRAM family: typically 0x50..0x57 depending on A0/A1/A2.
// Use the one matching your wiring.
#ifndef CFG_FRAM_ADDR7
#define CFG_FRAM_ADDR7      (0x50u)
#endif


// ---- I2C bus wiring (board) ----
#define CFG_I2C_PORT        (I2C_NUM_0)
#define CFG_I2C_SDA_GPIO    (GPIO_NUM_8)
#define CFG_I2C_SCL_GPIO    (GPIO_NUM_9)
#define CFG_I2C_CLK_HZ      (400000u)      // si tu considères que c'est imposé par ton HW

// -------------------------
// Frozen hardware config
// -------------------------
typedef struct {
    // ---- I2C bus wiring ----
    i2c_port_t i2c_port;
    gpio_num_t i2c_sda_gpio;
    gpio_num_t i2c_scl_gpio;
    uint32_t   i2c_clk_hz;          // 100k/400k...
    bool       i2c_pullups_en;      // internal pullups (usually false if you have external pullups)

    // ---- I2C slave addresses ----
    uint8_t    pca9536_addr7;
    uint8_t    pcf8523_addr7;
    uint8_t    fram_addr7;

} system_cfg_t;

/**
 * @brief Global frozen config instance.
 * Define it in system_cfg.c as:
 *   const system_cfg_t g_hw_cfg = { ... };
 */
extern const system_cfg_t g_hw_cfg;

/**
 * @brief Optional: validate config sanity (called once at boot).
 * @return ESP_OK if config looks sane, else ESP_ERR_INVALID_ARG.
 */
esp_err_t system_cfg_validate(const system_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
