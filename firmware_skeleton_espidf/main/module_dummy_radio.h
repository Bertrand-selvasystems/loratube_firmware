#pragma once
/**
 * @file module_dummy_radio.h
 * @brief Dummy radio driver example (NO FreeRTOS). Simulates a TX primitive.
 *
 * Allowed:
 *  - ESP-IDF headers, esp_err_t, (optional) ESP_LOGx
 *
 * Forbidden:
 *  - FreeRTOS includes, queues/eventbits
 *  - system_* includes
 *
 * Example:
 *  typedef struct { int foo; } dummy_radio_cfg_t;
 *  typedef struct { dummy_radio_cfg_t cfg; bool inited; } dummy_radio_t;
 *
 *  esp_err_t dummy_radio_init(dummy_radio_t *r, const dummy_radio_cfg_t *cfg);
 *  esp_err_t dummy_radio_send(dummy_radio_t *r, const void *data, size_t len);
 *  void      dummy_radio_deinit(dummy_radio_t *r);
 */

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t bitrate;
} dummy_radio_cfg_t;

typedef struct {
    dummy_radio_cfg_t cfg;
    bool inited;
} dummy_radio_t;

esp_err_t dummy_radio_init(dummy_radio_t *r, const dummy_radio_cfg_t *cfg);
esp_err_t dummy_radio_send(dummy_radio_t *r, const void *data, size_t len);
void      dummy_radio_deinit(dummy_radio_t *r);

#ifdef __cplusplus
}
#endif
