/**
 * @file    module_i2c_bus.c
 * @brief   I2C bus primitives (master) — RTOS-free driver layer.
 *
 * Design rules:
 * - No FreeRTOS primitives here (no mutex/queue/eventbits, no pdMS_TO_TICKS()).
 * - Timeouts are provided in ticks by tasks: i2c_timeout_ticks_t.
 * - No retry/delay policy: tasks own behavior.
 */

 /** on s'appuie sur la gestion du bus lui meme sur l'accès concurrent à la liaison I2C, pas de surcouche ici */

#include "module_i2c_bus.h"

#include <string.h>

#include "esp_check.h"

// TAG used only for esp_check messages (no logging policy here)
#define TAG "I2C_BUS"

static inline TickType_t ticks_to_idf(i2c_timeout_ticks_t t)
{
    // ESP-IDF expects TickType_t; we keep API as uint32_t ticks.
    // 0 means "no wait" (immediate). If you want "wait forever", pass portMAX_DELAY from task.
    // permet de chasser freertos du module driver
    return (TickType_t)t;
}

esp_err_t i2c_bus_init(const i2c_bus_cfg_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "cfg NULL");
    ESP_RETURN_ON_FALSE(cfg->clk_speed_hz > 0, ESP_ERR_INVALID_ARG, TAG, "clk_speed_hz=0");

    i2c_config_t icfg = {0};
    icfg.mode = I2C_MODE_MASTER;
    icfg.sda_io_num = cfg->sda_gpio;
    icfg.scl_io_num = cfg->scl_gpio;
    icfg.sda_pullup_en = cfg->enable_pullups ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    icfg.scl_pullup_en = cfg->enable_pullups ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    icfg.master.clk_speed = cfg->clk_speed_hz;

    esp_err_t ret = i2c_param_config(cfg->port, &icfg);
    if (ret != ESP_OK) return ret;

    // Master mode: RX/TX buffers = 0
    ret = i2c_driver_install(cfg->port, icfg.mode, 0, 0, 0);
    return ret;
}

esp_err_t i2c_bus_deinit(i2c_port_t port)
{
    return i2c_driver_delete(port);
}

esp_err_t i2c_bus_write(i2c_port_t port, uint8_t addr7,
                        const uint8_t *data, size_t len,
                        i2c_timeout_ticks_t timeout_ticks)
{
    ESP_RETURN_ON_FALSE(data || len == 0, ESP_ERR_INVALID_ARG, TAG, "data NULL");
    return i2c_master_write_to_device(port, addr7, data, len, ticks_to_idf(timeout_ticks));
}

esp_err_t i2c_bus_read(i2c_port_t port, uint8_t addr7,
                       uint8_t *data, size_t len,
                       i2c_timeout_ticks_t timeout_ticks)
{
    ESP_RETURN_ON_FALSE(data || len == 0, ESP_ERR_INVALID_ARG, TAG, "data NULL");
    return i2c_master_read_from_device(port, addr7, data, len, ticks_to_idf(timeout_ticks));
}

esp_err_t i2c_bus_write_read(i2c_port_t port, uint8_t addr7,
                            const uint8_t *w, size_t wlen,
                            uint8_t *r, size_t rlen,
                            i2c_timeout_ticks_t timeout_ticks)
{
    ESP_RETURN_ON_FALSE((w || wlen == 0), ESP_ERR_INVALID_ARG, TAG, "w NULL");
    ESP_RETURN_ON_FALSE((r || rlen == 0), ESP_ERR_INVALID_ARG, TAG, "r NULL");

    return i2c_master_write_read_device(port, addr7,
                                        w, wlen,
                                        r, rlen,
                                        ticks_to_idf(timeout_ticks));
}
