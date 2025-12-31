#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SemaphoreHandle_t g_i2c_mutex;

esp_err_t system_sync_init(void);
void      system_sync_deinit(void);
void      system_sync_dump(void);

#ifdef __cplusplus
}
#endif
