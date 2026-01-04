/* main/initialisation.h */
#pragma once

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

#ifdef __cplusplus
}
#endif
