#pragma once
/**
 * @file system_faults.h
 * @brief Fault bitmap + rules. Centralize who sets/clears faults.
 *
 * Idea:
 *  - faults are not "logs": they are machine-readable health state
 *  - faults may be latched (sticky) or transient (clearable)
 *
 * Allowed:
 *  - FreeRTOS critical sections for atomic bitmap updates
 *  - system_state integration (e.g., set EG_STATE_FAULT_PRESENT)
 *
 * Forbidden:
 *  - hardware access
 *  - being called from ISR unless explicitly ISR-safe (not in this v1)
 *
 * Example:
 *  system_faults_set(FAULT_I2C_INIT);
 *  if (system_faults_any()) { ... }
 */

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FAULT_NONE = 0,

    FAULT_SELFTEST_FAILED = 1,
    FAULT_QUEUE_INIT      = 2,
    FAULT_STATE_INIT      = 3,

    FAULT_DRIVER_SENSOR   = 10,
    FAULT_DRIVER_RADIO    = 11,
} system_fault_id_t;

esp_err_t system_faults_init(void);
void      system_faults_deinit(void);

void      system_faults_set(system_fault_id_t id);
void      system_faults_clear(system_fault_id_t id);
bool      system_faults_is_set(system_fault_id_t id);

bool      system_faults_any(void);
uint32_t  system_faults_bitmap(void);

#ifdef __cplusplus
}
#endif
