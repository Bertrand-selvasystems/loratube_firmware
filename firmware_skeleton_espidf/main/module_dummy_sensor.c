/**
 * @file module_dummy_sensor.c
 * @brief Dummy sensor driver implementation (NO FreeRTOS).
 */

#include "module_dummy_sensor.h"
#include "esp_timer.h"

esp_err_t dummy_sensor_init(dummy_sensor_t *dev, const dummy_sensor_cfg_t *cfg)
{
    if (!dev || !cfg) return ESP_ERR_INVALID_ARG;
    dev->cfg = *cfg;
    dev->seq = 0;
    dev->inited = true;
    return ESP_OK;
}

esp_err_t dummy_sensor_read(dummy_sensor_t *dev, float *out_value)
{
    if (!dev || !out_value) return ESP_ERR_INVALID_ARG;
    if (!dev->inited) return ESP_ERR_INVALID_STATE;

    uint64_t us = esp_timer_get_time();
    dev->seq++;

    // Deterministic-ish dummy value
    float x = (float)((us / 1000ull) % 1000ull);
    *out_value = dev->cfg.slope * x + dev->cfg.offset + (float)(dev->seq % 10u) * 0.1f;
    return ESP_OK;
}

void dummy_sensor_deinit(dummy_sensor_t *dev)
{
    if (!dev) return;
    dev->inited = false;
}
