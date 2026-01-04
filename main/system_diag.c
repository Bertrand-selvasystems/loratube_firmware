#include "system_diag.h"

#include "esp_log.h"
#include "esp_private/esp_clk.h"


static const char *TAG = "DIAG";

void system_diag_boot_dump(void)
{
    const uint32_t cpu_mhz = (uint32_t)(esp_clk_cpu_freq() / 1000000U);
    ESP_LOGI(TAG, "CPU freq = %u MHz", (unsigned)cpu_mhz);
}
