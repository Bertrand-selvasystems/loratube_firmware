#pragma once
/**
 * @file task_measure.h
 * @brief Measure task (service). Owns the measurement pipeline.
 *
 * This is a FreeRTOS actor:
 *  - calls the driver (module_dummy_sensor)
 *  - publishes results to queues
 *  - sets synchronization bits
 *
 * Allowed:
 *  - FreeRTOS (tasks/queues/event groups)
 *  - system_* modules (queues/state/faults)
 *  - module_* drivers
 *
 * Forbidden:
 *  - direct access by non-owner code to the hardware driver
 *
 * Typical pattern:
 *  void task_fn(void *arg) {
 *    for (;;) {
 *      read driver
 *      xQueueOverwrite(q_latest, &msg);   // depth=1
 *      xEventGroupSetBits(eg_state, EG_STATE_MEASURE_DONE);
 *      vTaskDelay(pdMS_TO_TICKS(period_ms));
 *    }
 *  }
 */

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t task_measure_start(void);

#ifdef __cplusplus
}
#endif
