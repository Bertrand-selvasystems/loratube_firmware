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

//#define FEATURE_SELFTEST            (1)   /* set to 0 for production builds */

// #define TASK_PRIO_MEASURE           (5)
// #define TASK_STACK_MEASURE          (3072)

// #define TASK_PRIO_RADIO             (6)
// #define TASK_STACK_RADIO            (4096)

// #define RADIO_CMD_QUEUE_DEPTH       (8u)
// #define MEASURE_PERIOD_MS           (1000u)
// #define MEASURE_READS_PER_CYCLE      (3u)
// #define MEASURE_INTERREAD_DELAY_MS   (50u)

// #define QUEUE_SEND_TIMEOUT_MS       (50u)
#define POLICY_I2C_TIMEOUT_MS   (50u)
#define POLICY_I2C_LOCK_MS      (50u)

#define TASK_PRIO_I2C           (5)
#define TASK_STACK_I2C          (2048)

#define FIRMWARE_VERSION        (1)

// profondeur de la queue pour les logs
#define SYS_Q_LOG_DEPTH          8
// profondeur de la queue pour le port I2C
#define SYS_Q_I2C_CMD_DEPTH  8
// profondeur de la queue de commande pour le PCA
#define SYS_Q_PCA_CMD_DEPTH 8
// profondeur de la queue de commande pour le RTC
#define SYS_Q_RTC_CMD_DEPTH 8

#define SYS_TASK_I2C_STACK  2048
#define SYS_TASK_I2C_PRIO   6

#define SYS_TASK_PCA_STACK  2048
#define SYS_TASK_PCA_PRIO   7

#define SYS_TASK_RTC_STACK   3072
#define SYS_TASK_RTC_PRIO    8
