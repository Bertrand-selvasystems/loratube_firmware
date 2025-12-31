#pragma once
/**
 * @file test_selftest.h
 * @brief DEV-only selftest hooks. Compiled only when FEATURE_SELFTEST=1.
 *
 * Goal:
 *  - keep all ad-hoc tests in ONE place
 *  - do not pollute production code paths
 *
 * Integration pattern (initialisation.c):
 *  #if FEATURE_SELFTEST
 *      ESP_ERROR_CHECK(test_selftest_run(&sys));
 *  #endif
 *
 * What it can do:
 *  - check queues/eventgroups exist
 *  - demonstrate overwrite queue vs FIFO queue usage
 *  - run lightweight driver checks
 *
 * Why excluded from production:
 *  - may increase boot time
 *  - may touch hardware in non-operational ways
 *  - increases code size and attack surface
 */

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct system_s system_t;   // forward (defined in initialisation.h)

esp_err_t test_selftest_run(system_t *sys);

#ifdef __cplusplus
}
#endif
