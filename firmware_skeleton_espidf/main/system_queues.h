#pragma once
/**
 * @file system_queues.h
 * @brief Global system queues: declaration + init/deinit + debug dumps.
 *
 * Allowed:
 *  - FreeRTOS queue API
 *  - system_types.h
 *
 * Forbidden:
 *  - any hardware driver includes
 *  - being included from drivers (module_*.c)
 *
 * Patterns:
 *  1) "Latest value" queue (depth=1) => overwrite semantics
 *     QueueHandle_t q = xQueueCreate(1, sizeof(msg_t));
 *     xQueueOverwrite(q, &msg);          // only valid for depth=1
 *     xQueueReceive(q, &out, pdMS_TO_TICKS(10));
 *
 *  2) FIFO queue (depth>1) => normal send semantics
 *     QueueHandle_t q = xQueueCreate(8, sizeof(evt_t));
 *     xQueueSend(q, &evt, pdMS_TO_TICKS(50));    // blocks up to 50ms
 *     xQueueReceive(q, &evt, portMAX_DELAY);     // consumer blocks
 *
 *  3) ISR-safe enqueue
 *     BaseType_t hpw = pdFALSE;
 *     xQueueSendFromISR(q, &evt, &hpw);
 *     portYIELD_FROM_ISR(hpw);
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"

#include "system_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern QueueHandle_t q_measure_latest;   // depth=1, overwrite
extern QueueHandle_t q_radio_cmd;        // depth>1, FIFO

esp_err_t system_queues_init(void);
void      system_queues_deinit(void);
void      system_queues_dump(void);

#ifdef __cplusplus
}
#endif
