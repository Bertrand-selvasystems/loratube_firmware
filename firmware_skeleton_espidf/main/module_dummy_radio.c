/**
 * @file module_dummy_radio.c
 * @brief Dummy radio driver implementation (NO FreeRTOS).
 */

#include "module_dummy_radio.h"
#include "esp_log.h"

static const char *TAG = "DUMMY_RADIO";

esp_err_t dummy_radio_init(dummy_radio_t *r, const dummy_radio_cfg_t *cfg)
{
    if (!r || !cfg) return ESP_ERR_INVALID_ARG;
    r->cfg = *cfg;
    r->inited = true;
    ESP_LOGI(TAG, "init bitrate=%lu", (unsigned long)r->cfg.bitrate);
    return ESP_OK;
}

esp_err_t dummy_radio_send(dummy_radio_t *r, const void *data, size_t len)
{
    if (!r || !data || len == 0) return ESP_ERR_INVALID_ARG;
    if (!r->inited) return ESP_ERR_INVALID_STATE;

    // simulate TX
    ESP_LOGI(TAG, "TX %u bytes (first=0x%02x)", (unsigned)len, ((const uint8_t*)data)[0]);
    return ESP_OK;
}

void dummy_radio_deinit(dummy_radio_t *r)
{
    if (!r) return;
    r->inited = false;
}
