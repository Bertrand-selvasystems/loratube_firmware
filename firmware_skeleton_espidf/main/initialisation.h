#pragma once
/**
 * @file initialisation.h
 * @brief Boot orchestration: start system layers in the correct order.
 *
 * Rules:
 *  - no business logic here
 *  - only orchestration + error checking
 *  - config is frozen (system_cfg.*), runtime lives in system_t
 *
 * Typical order:
 *  1) system_state_init()
 *  2) system_faults_init()
 *  3) system_queues_init()
 *  4) module init
 *  5) (optional) selftest
 *  6) task start
 *
 * Integration of DEV selftest:
 *  #if FEATURE_SELFTEST
 *      ESP_ERROR_CHECK(test_selftest_run(&sys));
 *  #endif
 */

#include "esp_err.h"
#include "system_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct system_s {
    const system_cfg_t *cfg;   // frozen config (const pointer)
    bool init_done;
} system_t;

extern system_t sys;

esp_err_t initialisation_start(void);

#ifdef __cplusplus
}
#endif
