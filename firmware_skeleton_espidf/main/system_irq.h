#pragma once
/**
 * @file system_irq.h
 * @brief ISR -> task bridge helpers. Standardize ISR-safe notification patterns.
 *
 * This file is about "wiring interrupts into the FreeRTOS world", not about business logic.
 *
 * Allowed:
 *  - ISR-safe primitives: xQueueSendFromISR, vTaskNotifyGiveFromISR, xTaskNotifyFromISR
 *  - portYIELD_FROM_ISR
 *
 * Forbidden:
 *  - heavy logging in ISR
 *  - malloc/free in ISR
 *  - any blocking call in ISR
 *
 * Example (queue from ISR):
 *  BaseType_t hpw = pdFALSE;
 *  my_evt_t evt = { ... };
 *  xQueueSendFromISR(q_irq, &evt, &hpw);
 *  portYIELD_FROM_ISR(hpw);
 *
 * Example (direct-to-task notification from ISR):
 *  BaseType_t hpw = pdFALSE;
 *  vTaskNotifyGiveFromISR(task_handle, &hpw);
 *  portYIELD_FROM_ISR(hpw);
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Skeleton: declare IRQ-related queues/task handles here if needed later. */
void system_irq_init(void);
void system_irq_deinit(void);

#ifdef __cplusplus
}
#endif
