/**
 * @file system_faults.c
 * @brief Fault bitmap implementation.
 */

#include "system_faults.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "system_state.h"

static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;
static uint32_t s_faults = 0;

static inline uint32_t fault_bit(system_fault_id_t id)
{
    if (id <= 0) return 0;
    if ((int)id >= 32) return 0;   // keep v1 simple: up to 31 faults
    return (1u << (uint32_t)id);
}

esp_err_t system_faults_init(void)
{
    portENTER_CRITICAL(&s_mux);
    s_faults = 0;
    portEXIT_CRITICAL(&s_mux);
    return ESP_OK;
}

void system_faults_deinit(void)
{
    portENTER_CRITICAL(&s_mux);
    s_faults = 0;
    portEXIT_CRITICAL(&s_mux);
}

void system_faults_set(system_fault_id_t id)
{
    uint32_t b = fault_bit(id);
    if (!b) return;

    portENTER_CRITICAL(&s_mux);
    s_faults |= b;
    portEXIT_CRITICAL(&s_mux);

    if (eg_state) {
        xEventGroupSetBits(eg_state, EG_STATE_FAULT_PRESENT);
    }
}

void system_faults_clear(system_fault_id_t id)
{
    uint32_t b = fault_bit(id);
    if (!b) return;

    portENTER_CRITICAL(&s_mux);
    s_faults &= ~b;
    uint32_t now = s_faults;
    portEXIT_CRITICAL(&s_mux);

    if (eg_state && now == 0) {
        xEventGroupClearBits(eg_state, EG_STATE_FAULT_PRESENT);
    }
}

bool system_faults_is_set(system_fault_id_t id)
{
    uint32_t b = fault_bit(id);
    if (!b) return false;
    portENTER_CRITICAL(&s_mux);
    bool set = (s_faults & b) != 0;
    portEXIT_CRITICAL(&s_mux);
    return set;
}

bool system_faults_any(void)
{
    portENTER_CRITICAL(&s_mux);
    bool any = (s_faults != 0);
    portEXIT_CRITICAL(&s_mux);
    return any;
}

uint32_t system_faults_bitmap(void)
{
    portENTER_CRITICAL(&s_mux);
    uint32_t v = s_faults;
    portEXIT_CRITICAL(&s_mux);
    return v;
}
