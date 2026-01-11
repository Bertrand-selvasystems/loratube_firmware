#include "module_pcf8523.h"

static inline bool in_range_u8(uint8_t v, uint8_t lo, uint8_t hi) { return (v >= lo) && (v <= hi); }

uint8_t pcf8523_bin2bcd(uint8_t v) { return (uint8_t)(((v / 10) << 4) | (v % 10)); }
uint8_t pcf8523_bcd2bin(uint8_t v) { return (uint8_t)(((v >> 4) * 10) + (v & 0x0F)); }

esp_err_t pcf8523_encode_datetime(uint8_t out7[7], const rtc_datetime_t *dt)
{
    if (!out7 || !dt) return ESP_ERR_INVALID_ARG;

    // Minimal sanity (tu peux renforcer)
    if (!in_range_u8(dt->sec, 0, 59))  return ESP_ERR_INVALID_ARG;
    if (!in_range_u8(dt->min, 0, 59))  return ESP_ERR_INVALID_ARG;
    if (!in_range_u8(dt->hour,0, 23))  return ESP_ERR_INVALID_ARG;
    if (!in_range_u8(dt->day, 1, 31))  return ESP_ERR_INVALID_ARG;
    if (!in_range_u8(dt->wday,0, 6))   return ESP_ERR_INVALID_ARG;
    if (!in_range_u8(dt->month,1, 12)) return ESP_ERR_INVALID_ARG;
    if (dt->year < 2000 || dt->year > 2099) return ESP_ERR_INVALID_ARG;

    const uint8_t yy = (uint8_t)(dt->year - 2000);

    // Datasheet typically has flags in MSBs; we keep them 0
    out7[0] = pcf8523_bin2bcd(dt->sec)  & 0x7F; // seconds: bit7 often OS flag
    out7[1] = pcf8523_bin2bcd(dt->min)  & 0x7F;
    out7[2] = pcf8523_bin2bcd(dt->hour) & 0x3F; // 24h mode
    out7[3] = pcf8523_bin2bcd(dt->day)  & 0x3F;
    out7[4] = (dt->wday & 0x07);
    out7[5] = pcf8523_bin2bcd(dt->month) & 0x1F; // bit7 century in some RTCs
    out7[6] = pcf8523_bin2bcd(yy);

    return ESP_OK;
}

esp_err_t pcf8523_decode_datetime(rtc_datetime_t *dt, const uint8_t in7[7])
{
    if (!dt || !in7) return ESP_ERR_INVALID_ARG;

    const uint8_t sec  = pcf8523_bcd2bin(in7[0] & 0x7F);
    const uint8_t min  = pcf8523_bcd2bin(in7[1] & 0x7F);
    const uint8_t hour = pcf8523_bcd2bin(in7[2] & 0x3F);
    const uint8_t day  = pcf8523_bcd2bin(in7[3] & 0x3F);
    const uint8_t wday = (in7[4] & 0x07);
    const uint8_t mon  = pcf8523_bcd2bin(in7[5] & 0x1F);
    const uint8_t yy   = pcf8523_bcd2bin(in7[6]);

    if (!in_range_u8(sec, 0, 59))  return ESP_ERR_INVALID_RESPONSE;
    if (!in_range_u8(min, 0, 59))  return ESP_ERR_INVALID_RESPONSE;
    if (!in_range_u8(hour,0, 23))  return ESP_ERR_INVALID_RESPONSE;
    if (!in_range_u8(day, 1, 31))  return ESP_ERR_INVALID_RESPONSE;
    if (!in_range_u8(wday,0, 6))   return ESP_ERR_INVALID_RESPONSE;
    if (!in_range_u8(mon, 1, 12))  return ESP_ERR_INVALID_RESPONSE;

    dt->sec = sec;
    dt->min = min;
    dt->hour= hour;
    dt->day = day;
    dt->wday= wday;
    dt->month= mon;
    dt->year = (uint16_t)(2000 + yy);

    return ESP_OK;
}
