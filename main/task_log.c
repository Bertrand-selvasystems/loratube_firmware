#include "task_log.h"
#include "system_queues.h"
#include "system_types.h"
#include "system_cfg.h"        // PCB_VERSION etc (frozen config)
#include "system_config.h"     // FIRMWARE_VERSION etc (tuning/build)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

static const char *TAG = "LOG";

static void print_boot_banner(void)
{
    // Ici tu peux utiliser printf ou ESP_LOGI.
    // Si tu veux un log “propre” (timestamps), préfère ESP_LOGI.
    // Pour l’ASCII art, printf est souvent plus lisible.
    printf("__________________________________________________________________________\n\n");
    printf(" ____  ____  __    _  _   __     ____  _  _  ____  ____  ____  _  _  ____ \n");
    printf("/ ___)(  __)(  )  / )( \\ / _\\   / ___)( \\/ )/ ___)(_  _)(  __)( \\/ )/ ___)\n");
    printf("\\___ \\ ) _) / (_/\\\\ \\/ //    \\  \\___ \\ )  / \\___ \\  )(   ) _) / \\/ \\\\___ \\\n");
    printf("(____/(____)\\____/ \\__/ \\_/\\_/  (____/(__/  (____/ (__) (____)\\_)(_/(____/\n\n");
    printf("    LoRaTUBE\n");
    printf("    PCB Version      : %d\n", (int)PCB_VERSION);
    printf("    Firmware Version : %d\n", (int)FIRMWARE_VERSION);
    printf("__________________________________________________________________________\n\n");
    printf("DEBUT INITIALISATION :\n");
}

static void print_acq_start(void)
{
    printf("__________________________________________________________________________\n\n");
    printf("DEBUT ACQUISITION : \n");
}

static void task_log_main(void *arg)
{
    (void)arg;

    log_msg_t m;
    for (;;) {
        if (xQueueReceive(g_q_log, &m, portMAX_DELAY) == pdTRUE) {
            switch (m.cmd) {
                case LOG_CMD_BOOT_BANNER: print_boot_banner(); break;
                case LOG_CMD_ACQ_START:   print_acq_start();   break;
                default:
                    ESP_LOGW(TAG, "Unknown log cmd: %d", (int)m.cmd);
                    break;
            }
        }
    }
}

void task_log_start(void)
{
    xTaskCreate(task_log_main, "task_log", 4096, NULL, 5, NULL);
}
