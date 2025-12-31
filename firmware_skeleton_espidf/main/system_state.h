#pragma once
/**
 * @file system_state.h
 * @brief Global event groups: state + synchronization bits.
 *
 * Allowed:
 *  - FreeRTOS EventGroup API
 *  - init/deinit/dump
 *
 * Forbidden:
 *  - being included from drivers (module_*.c)
 *  - hardware access logic
 *
 * Example:
 *  eg_state = xEventGroupCreate();
 *
 *  // SET/CLEAR
 *  xEventGroupSetBits(eg_state, EG_STATE_SYSTEM_READY);
 *  xEventGroupClearBits(eg_state, EG_STATE_MEASURE_DONE);
 *
 *  // WAIT (any-of)
 *  EventBits_t b = xEventGroupWaitBits(
 *      eg_state,
 *      EG_STATE_MEASURE_DONE | EG_STATE_FAULT_PRESENT,
 *      pdFALSE,     // do not clear on exit
 *      pdFALSE,     // wait ANY bit
 *      pdMS_TO_TICKS(1000)
 *  );
 *
 *  // WAIT (all-of) + clear-on-exit
 *  xEventGroupWaitBits(eg_state, bits, pdTRUE, pdTRUE, portMAX_DELAY);
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

extern EventGroupHandle_t eg_state;

/* ---- eg_state bits (group by theme) ---- */
#define EG_STATE_SYSTEM_READY        (1u << 0)
#define EG_STATE_SELFTEST_OK         (1u << 1)

#define EG_STATE_MEASURE_DONE        (1u << 8)

#define EG_STATE_RADIO_TX_DONE       (1u << 12)

#define EG_STATE_FAULT_PRESENT       (1u << 31)

esp_err_t system_state_init(void);
void      system_state_deinit(void);
void      system_state_dump(void);

#ifdef __cplusplus
}
#endif
