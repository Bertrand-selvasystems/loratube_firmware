#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

// Doit être appelé une fois au boot (par app_init) après task_pca_start()
void api_pca_bind_queue(void *q_pca_cmd);

// Async (simple)
esp_err_t api_pca_set_dir_async(uint8_t pin, bool is_output, uint32_t to_ticks);
esp_err_t api_pca_set_level_async(uint8_t pin, bool level, uint32_t to_ticks);
esp_err_t api_pca_blink_start_async(uint8_t pin, uint32_t on_ms, uint32_t off_ms, bool start_high, uint32_t to_ticks);
esp_err_t api_pca_blink_stop_async(uint8_t pin, uint32_t to_ticks);

// Sync (optionnel) : attend que task_pca confirme
esp_err_t api_pca_set_level(uint8_t pin, bool level, uint32_t to_ticks);
