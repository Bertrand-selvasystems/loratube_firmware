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
    .g_i2c0_cfg = {
        .port = CFG_I2C_PORT,
        .sda_gpio = CFG_I2C_SDA_GPIO,
        .scl_gpio = CFG_I2C_SCL_GPIO,
        .clk_speed_hz = CFG_I2C_CLK_HZ,
        .enable_pullups = false, // ou true selon ton HW
    },

    .pca9536 = {
        .pin_e22  = 0,
        .pin_led1 = 1,
        .pin_led2 = 2,
        .pin_buck = 3,
        .boot_out4 = 0x01,  // E22=1, LED1=0, LED2=0, BUCK=0
    },

    .pca9536_addr7 = CFG_PCA9536_ADDR7,
    .pcf8523_addr7 = CFG_PCF8523_ADDR7,
    .fram_addr7    = CFG_FRAM_ADDR7,
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
    ESP_RETURN_ON_FALSE(cfg->g_i2c0_cfg.clk_speed_hz > 0,
                        ESP_ERR_INVALID_ARG, TAG, "i2c clk=0");

    // GPIOs
    ESP_RETURN_ON_FALSE((int)cfg->g_i2c0_cfg.sda_gpio >= 0,
                        ESP_ERR_INVALID_ARG, TAG, "bad SDA gpio");
    ESP_RETURN_ON_FALSE((int)cfg->g_i2c0_cfg.scl_gpio >= 0,
                        ESP_ERR_INVALID_ARG, TAG, "bad SCL gpio");
    ESP_RETURN_ON_FALSE(cfg->g_i2c0_cfg.sda_gpio != cfg->g_i2c0_cfg.scl_gpio,
                        ESP_ERR_INVALID_ARG, TAG, "SDA==SCL");

    // Addresses
    ESP_RETURN_ON_FALSE(is_valid_addr7(cfg->pca9536_addr7), ESP_ERR_INVALID_ARG, TAG, "bad PCA addr7");
    ESP_RETURN_ON_FALSE(is_valid_addr7(cfg->pcf8523_addr7), ESP_ERR_INVALID_ARG, TAG, "bad RTC addr7");
    ESP_RETURN_ON_FALSE(is_valid_addr7(cfg->fram_addr7),    ESP_ERR_INVALID_ARG, TAG, "bad FRAM addr7");

    // ---- PCA9536 mapping ----
    const pca9536_map_t *m = &cfg->pca9536;

    ESP_RETURN_ON_FALSE(m->pin_e22  < 4, ESP_ERR_INVALID_ARG, TAG, "bad PCA pin_e22");
    ESP_RETURN_ON_FALSE(m->pin_led1 < 4, ESP_ERR_INVALID_ARG, TAG, "bad PCA pin_led1");
    ESP_RETURN_ON_FALSE(m->pin_led2 < 4, ESP_ERR_INVALID_ARG, TAG, "bad PCA pin_led2");
    ESP_RETURN_ON_FALSE(m->pin_buck < 4, ESP_ERR_INVALID_ARG, TAG, "bad PCA pin_buck");

    // No duplicates: all four functions must map to distinct PCA bits
    uint8_t used = 0;
    used |= (uint8_t)(1u << m->pin_e22);
    used |= (uint8_t)(1u << m->pin_led1);
    used |= (uint8_t)(1u << m->pin_led2);
    used |= (uint8_t)(1u << m->pin_buck);
    ESP_RETURN_ON_FALSE(used == 0x0F, ESP_ERR_INVALID_ARG, TAG, "PCA pin map duplicates");

    // Boot output nibble sanity (optional but useful)
    ESP_RETURN_ON_FALSE((m->boot_out4 & 0xF0) == 0, ESP_ERR_INVALID_ARG, TAG, "PCA boot_out4 high bits");

    // E22 normal=1, BUCK normal=0
    #define BIT_IS_SET(x, b)   (((x) & (1u << (b))) != 0u)
    #define BIT_IS_CLEAR(x, b) (((x) & (1u << (b))) == 0u)

    ESP_RETURN_ON_FALSE(BIT_IS_SET(m->boot_out4, m->pin_e22),
                    ESP_ERR_INVALID_ARG, TAG, "PCA boot E22 must be 1");
    ESP_RETURN_ON_FALSE(BIT_IS_CLEAR(m->boot_out4, m->pin_buck),
                    ESP_ERR_INVALID_ARG, TAG, "PCA boot BUCK must be 0");


    return ESP_OK;
}
