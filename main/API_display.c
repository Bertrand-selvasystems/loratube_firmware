#include "API_display.h"
#include "system_queues.h"
#include "system_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static inline void send_log_cmd(log_cmd_t cmd, uint32_t arg)
{
    if (!g_q_log) return; // option: raise fault

    log_msg_t m = {
        .cmd = cmd,
        .arg_u32 = arg
    };

    // 0 tick : si queue pleine, on drop. C’est du log.
    (void)xQueueSend(g_q_log, &m, 0);
}

void API_display_boot_banner(void)
{
    send_log_cmd(LOG_CMD_BOOT_BANNER, 0);
}

void API_display_acquisition_start(void)
{
    send_log_cmd(LOG_CMD_ACQ_START, 0);
}
