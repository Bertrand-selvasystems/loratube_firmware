/**
 * @file    system_cfg.c
 * @brief   Frozen hardware configuration instance + sanity checks.
 *
 * Notes:
 * - g_hw_cfg is const: hardware wiring, addresses, bus speeds.
 * - No FreeRTOS here.
 */

#include "system_cfg.h"

#include "esp_check.h"

#define TAG "SYS_CFG"

// -------------------------
// Global frozen instance
// -------------------------
// TODO: adjust GPIO numbers and I2C port for your board.
const system_cfg_t g_hw_cfg = {
    // ---- I2C bus wiring ----
    .i2c_port            = CFG_I2C_PORT,
    .i2c_sda_gpio        = CFG_I2C_SDA_GPIO,     // TODO
    .i2c_scl_gpio        = CFG_I2C_SCL_GPIO,     // TODO
    .i2c_clk_hz          = CFG_I2C_CLK_HZ,        // 100000u or 400000u
    .i2c_pullups_en      = false,          // usually false if you have external pullups

    // ---- I2C slave addresses ----
    .pca9536_addr7       = CFG_PCA9536_ADDR7,
    .pcf8523_addr7       = CFG_PCF8523_ADDR7,
    .fram_addr7          = CFG_FRAM_ADDR7,

};

static bool is_valid_addr7(uint8_t a)
{
    // Reject reserved 0x00..0x07 and 0x78..0x7F
    return (a >= 0x08u) && (a <= 0x77u);
}

esp_err_t system_cfg_validate(const system_cfg_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "cfg NULL");

    // I2C basic
    ESP_RETURN_ON_FALSE(cfg->i2c_clk_hz > 0, ESP_ERR_INVALID_ARG, TAG, "i2c_clk_hz=0");

    // GPIOs (basic sanity; refine if you want to forbid GPIO_NUM_NC etc.)
    ESP_RETURN_ON_FALSE(cfg->i2c_sda_gpio >= 0, ESP_ERR_INVALID_ARG, TAG, "bad SDA gpio");
    ESP_RETURN_ON_FALSE(cfg->i2c_scl_gpio >= 0, ESP_ERR_INVALID_ARG, TAG, "bad SCL gpio");
    ESP_RETURN_ON_FALSE(cfg->i2c_sda_gpio != cfg->i2c_scl_gpio, ESP_ERR_INVALID_ARG, TAG, "SDA==SCL");

    // Addresses
    ESP_RETURN_ON_FALSE(is_valid_addr7(cfg->pca9536_addr7), ESP_ERR_INVALID_ARG, TAG, "bad PCA addr7");
    ESP_RETURN_ON_FALSE(is_valid_addr7(cfg->pcf8523_addr7), ESP_ERR_INVALID_ARG, TAG, "bad RTC addr7");
    ESP_RETURN_ON_FALSE(is_valid_addr7(cfg->fram_addr7), ESP_ERR_INVALID_ARG, TAG, "bad FRAM addr7");

    return ESP_OK;
}
