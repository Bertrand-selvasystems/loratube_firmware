// task_rtc.h
#pragma once

#include <stdint.h>

void task_rtc_start(uint8_t addr7);
void *task_rtc_get_queue(void);   // pour bind l'API proprement
