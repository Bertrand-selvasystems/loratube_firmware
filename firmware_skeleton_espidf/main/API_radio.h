#pragma once
/**
 * @file API_radio.h
 * @brief Intent-level API for radio commands. Talks to the radio task via queues.
 *
 * Forbidden:
 *  - including module_* drivers
 *  - touching hardware directly
 *
 * Examples:
 *  API_radio_send_ping();
 *  API_radio_send_text("hello");
 */

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t API_radio_send_ping(void);
esp_err_t API_radio_send_text(const char *text);

#ifdef __cplusplus
}
#endif
