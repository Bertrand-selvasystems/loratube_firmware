#pragma once
/**
 * @file API_measure.h
 * @brief Intent-level API for measurements. Talks to tasks/queues, never to hardware.
 *
 * Allowed:
 *  - FreeRTOS queues (or notifications) to talk to tasks
 *  - system_types + system_queues/system_state
 *
 * Forbidden:
 *  - including module_* drivers
 *  - touching hardware directly
 *
 * Example:
 *  measure_msg_t last;
 *  if (API_measure_try_get_latest(&last, 0) == ESP_OK) { ... }
 */

#include "esp_err.h"
#include "system_types.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t API_measure_try_get_latest(measure_msg_t *out, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
