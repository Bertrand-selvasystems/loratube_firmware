/**
 * @file API_measure.c
 * @brief Measurement API implementation (queue consumer side).
 */

#include "API_measure.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "system_queues.h"

esp_err_t API_measure_try_get_latest(measure_msg_t *out, uint32_t timeout_ms)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    if (!q_measure_latest) return ESP_ERR_INVALID_STATE;

    TickType_t t = pdMS_TO_TICKS(timeout_ms);
    if (xQueueReceive(q_measure_latest, out, t) == pdTRUE) return ESP_OK;
    return ESP_ERR_TIMEOUT;
}
