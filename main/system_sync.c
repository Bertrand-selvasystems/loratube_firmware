#include "system_sync.h"
#include "esp_log.h"

static const char *TAG = "SYNC";

SemaphoreHandle_t g_i2c_mutex = NULL;

esp_err_t system_sync_init(void)
{
    if (g_i2c_mutex) return ESP_ERR_INVALID_STATE;

    g_i2c_mutex = xSemaphoreCreateMutex();
    if (!g_i2c_mutex) return ESP_ERR_NO_MEM;

    system_sync_dump();
    return ESP_OK;
}

void system_sync_deinit(void)
{
    if (g_i2c_mutex) {
        vSemaphoreDelete(g_i2c_mutex);
        g_i2c_mutex = NULL;
    }
}

void system_sync_dump(void)
{
    ESP_LOGI(TAG, "g_i2c_mutex=%p", (void*)g_i2c_mutex);
}
