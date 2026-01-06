#pragma once
#include <stdint.h>

void task_pca_start(uint8_t pca_addr7);
void *task_pca_get_queue(void);   // pour bind l'API proprement
