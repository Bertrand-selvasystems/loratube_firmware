#pragma once
/**
 * @file task_radio.h
 * @brief Radio task (service). Owns the radio driver and command queue consumption.
 *
 * Patterns:
 *  - FIFO command queue (depth>1):
 *      xQueueReceive(q_radio_cmd, &cmd, portMAX_DELAY);
 *  - Execute command by calling module_* driver
 *  - Signal completion via event bits
 */

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t task_radio_start(void);

#ifdef __cplusplus
}
#endif
