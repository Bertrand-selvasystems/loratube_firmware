#pragma once
/**
 * @file system_types.h
 * @brief Inter-module contracts: shared message types and enums.
 *
 * Allowed:
 *  - typedef struct / enum
 *  - shared #define (protocol values)
 *
 * Forbidden (by design):
 *  - FreeRTOS types (QueueHandle_t, EventGroupHandle_t, ...)
 *  - ESP-IDF driver headers
 *  - esp_err_t
 *  - functions or globals
 *
 * Examples:
 *  typedef struct { float temp_c; } sensor_temp_msg_t;
 *  typedef struct { float vbat_v; } vbat_msg_t;
 */

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float value;
    uint32_t t_ms;
} measure_msg_t;

typedef enum {
    RADIO_CMD_NOP = 0,
    RADIO_CMD_SEND_PING = 1,
    RADIO_CMD_SEND_TEXT = 2,
} radio_cmd_kind_t;

typedef struct {
    radio_cmd_kind_t kind;
    uint32_t arg_u32;
    char text[64];     /* optional payload */
} radio_cmd_t;
