#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "rtc_types.h"


#ifdef __cplusplus
extern "C" {
#endif




// ---- PCF8523 register map (subset) ----
#define PCF8523_REG_CTRL1        0x00
#define PCF8523_REG_CTRL2        0x01
#define PCF8523_REG_CTRL3        0x02

#define PCF8523_REG_SECONDS      0x03
#define PCF8523_REG_MINUTES      0x04
#define PCF8523_REG_HOURS        0x05
#define PCF8523_REG_DAYS         0x06
#define PCF8523_REG_WEEKDAYS     0x07
#define PCF8523_REG_MONTHS       0x08
#define PCF8523_REG_YEARS        0x09

#define PCF8523_REG_TMR_CLKOUT   0x0F

// ---- CTRL2 bits (you already commented the meaning) ----
#define PCF8523_CTRL2_TI_TP   (1u << 7)
#define PCF8523_CTRL2_TF      (1u << 6)
#define PCF8523_CTRL2_AF      (1u << 5)
#define PCF8523_CTRL2_SI      (1u << 4)
#define PCF8523_CTRL2_MI      (1u << 3)
#define PCF8523_CTRL2_AIE     (1u << 2)
#define PCF8523_CTRL2_TIE     (1u << 1)
#define PCF8523_CTRL2_CTS     (1u << 0)

// ---- Tmr_CLKOUT_ctrl (0x0F) bits ----
// [7]=TAM [6]=TBM [5:3]=COF [2:1]=TAC [0]=TBC
#define PCF8523_CLKOUT_COF_OFF   (0x07u)   // COF=111
#define PCF8523_TMRCLKOUT_VAL_OFF  (uint8_t)((PCF8523_CLKOUT_COF_OFF << 3)) // -> 0x38

uint8_t pcf8523_bin2bcd(uint8_t v);
uint8_t pcf8523_bcd2bin(uint8_t v);
esp_err_t pcf8523_encode_datetime(uint8_t out7[7], const rtc_datetime_t *dt);
esp_err_t pcf8523_decode_datetime(rtc_datetime_t *dt, const uint8_t in7[7]);

#ifdef __cplusplus
}
#endif
