/**
 * @file system_state.c
 * @brief Implementation of global event groups.
 */

#include "system_state.h"
#include "esp_log.h"

static const char *TAG = "SYS_STATE";

EventGroupHandle_t eg_state = NULL;

esp_err_t system_state_init(void)
{
    if (eg_state) return ESP_ERR_INVALID_STATE;
    eg_state = xEventGroupCreate();
    if (!eg_state) return ESP_ERR_NO_MEM;
    system_state_dump();
    return ESP_OK;
}

void system_state_deinit(void)
{
    if (eg_state) { vEventGroupDelete(eg_state); eg_state = NULL; }
}

void system_state_dump(void)
{
    if (!eg_state) {
        ESP_LOGW(TAG, "eg_state=NULL");
        return;
    }
    EventBits_t b = xEventGroupGetBits(eg_state);
    ESP_LOGI(TAG, "eg_state=%p bits=0x%08lx", (void*)eg_state, (unsigned long)b);
}
