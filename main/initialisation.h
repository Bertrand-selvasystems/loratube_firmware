/* main/initialisation.h */
#pragma once
#include "task_i2c_types.h"   // pour i2c_xfer_t 
#include "xfer_i2c.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Boot orchestration entry point.
 *
 * Policy:
 * - app_main() calls this once, then can delete itself.
 * - This function owns the init order (queues/state/tasks/selftest).
 */
void initialisation_start(void);
const i2c_xfer_t* system_get_i2c_xfer(void);

#ifdef __cplusplus
}
#endif
