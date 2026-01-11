#pragma once
#include <stdint.h>

typedef struct {
    uint8_t sec;   // 0..59
    uint8_t min;   // 0..59
    uint8_t hour;  // 0..23
    uint8_t day;   // 1..31
    uint8_t wday;  // 0..6
    uint8_t month; // 1..12
    uint16_t year; // ex: 2026
} rtc_datetime_t;
