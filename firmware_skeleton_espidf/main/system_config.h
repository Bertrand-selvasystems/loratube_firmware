#pragma once
/**
 * @file system_config.h
 * @brief Software tuning knobs (NOT hardware wiring). Centralize all "magic numbers".
 *
 * Allowed:
 *  - task priorities, stack sizes
 *  - buffer sizes, timeouts, periods
 *  - feature flags (DEV only)
 *
 * Forbidden:
 *  - hardware pins / I2C addresses (go to system_cfg.*)
 *  - runtime variables or functions
 *
 * Example:
 *  #define TASK_PRIO_MEASURE   (5)
 *  #define TASK_STACK_MEASURE  (3072)
 *  #define MEASURE_PERIOD_MS   (1000u)
 *  #define FEATURE_SELFTEST    (1)   // DEV only
 */

#define FEATURE_SELFTEST            (1)   /* set to 0 for production builds */

#define TASK_PRIO_MEASURE           (5)
#define TASK_STACK_MEASURE          (3072)

#define TASK_PRIO_RADIO             (6)
#define TASK_STACK_RADIO            (4096)

#define RADIO_CMD_QUEUE_DEPTH       (8u)
#define MEASURE_PERIOD_MS           (1000u)
#define MEASURE_READS_PER_CYCLE      (3u)
#define MEASURE_INTERREAD_DELAY_MS   (50u)

#define QUEUE_SEND_TIMEOUT_MS       (50u)
