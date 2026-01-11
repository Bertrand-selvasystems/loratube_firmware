// api_rtc.h
#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "module_pcf8523.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "rtc_types.h"



#ifdef __cplusplus
extern "C" {
#endif

void api_rtc_bind_queue(QueueHandle_t q);

esp_err_t api_rtc_init_irq_timer(uint32_t to_ticks);
esp_err_t api_rtc_make_sqw_hi_z(uint32_t to_ticks);

esp_err_t api_rtc_get_datetime(rtc_datetime_t *out_dt, uint32_t to_ticks);
esp_err_t api_rtc_set_datetime(const rtc_datetime_t *dt, uint32_t to_ticks);

#ifdef __cplusplus
}
#endif
