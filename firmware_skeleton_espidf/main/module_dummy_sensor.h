#pragma once
/**
 * @file module_dummy_sensor.h
 * @brief Dummy driver example (NO FreeRTOS). Pretends to read a sensor.
 *
 * Allowed:
 *  - ESP-IDF driver headers (gpio/i2c/uart/adc/esp_timer...)
 *  - esp_err_t + minimal ESP_LOGx (optional)
 *
 * Forbidden:
 *  - FreeRTOS includes (tasks/queues/event groups)
 *  - system_* includes
 *
 * Typical driver API:
 *  typedef struct { ... } dummy_sensor_cfg_t;
 *  typedef struct { ... } dummy_sensor_t;
 *
 *  esp_err_t dummy_sensor_init(dummy_sensor_t *dev, const dummy_sensor_cfg_t *cfg);
 *  esp_err_t dummy_sensor_read(dummy_sensor_t *dev, float *out_value);
 *  void      dummy_sensor_deinit(dummy_sensor_t *dev);
 */

#include <stdint.h>
#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float slope;
    float offset;
} dummy_sensor_cfg_t;

typedef struct {
    dummy_sensor_cfg_t cfg;
    uint32_t seq;
    bool inited;
} dummy_sensor_t;

esp_err_t dummy_sensor_init(dummy_sensor_t *dev, const dummy_sensor_cfg_t *cfg);
esp_err_t dummy_sensor_read(dummy_sensor_t *dev, float *out_value);
void      dummy_sensor_deinit(dummy_sensor_t *dev);

#ifdef __cplusplus
}
#endif
