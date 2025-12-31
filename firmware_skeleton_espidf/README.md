# firmware_skeleton_espidf

A **firmware skeleton** for **ESP-IDF + FreeRTOS** projects.

This repository is **not** a framework and **not** a driver library.  
It is a disciplined starting point: **clear layers, explicit ownership, no circular dependencies**.

## Philosophy

**Rules (non-negotiable):**

1. **Drivers (`module_*.c`) are self-contained.**
   - No FreeRTOS includes
   - No queues
   - No event bits
   - They expose a clean C API: `init/read/write/deinit`.

2. **Tasks (`task_*.c`) orchestrate hardware access through drivers.**  
   Tasks are the **only** place where the system:
   - interacts with FreeRTOS primitives (queues, event groups, notifications)
   - applies retry/delay logic
   - sets/clears/waits event bits
   - manages health/faults

3. **APIs (`API_*.c`) talk to tasks, not to hardware.**  
   They provide *intent-level* functions and hide internal queues/events from the rest of the app.

4. **Configuration is split in two:**
   - **Frozen hardware config** (`system_cfg.*`): wiring, addresses, frequencies (**const, never modified**)
   - **Daily life** (runtime): handles, flags, counters, queues, event-groups

## Layers & dependencies

```
app_main / initialisation
        |
        v
     API_*             (façade: intent-level calls)
        |
        v
     task_*            (FreeRTOS actors: ownership + logic)
        |
        v
    module_*           (drivers: hardware access only)
```

**Allowed dependencies:**
- `module_*` -> ESP-IDF drivers only (I2C/UART/GPIO...), **no system_***
- `task_*`   -> `system_types`, `system_queues`, `system_state`, `system_faults`, `module_*`
- `API_*`    -> task-facing command queues / `system_*` (never hardware)

## Self-test (DEV only)

This skeleton supports optional self-tests compiled into the firmware.

- **How to enable:** set `FEATURE_SELFTEST` to `1` in `main/system_config.h`
- **What it does (v1):**
  - verifies queues and event-groups exist
  - demonstrates queue overwrite vs FIFO patterns
  - runs dummy driver checks
  - sets `EG_STATE_SELFTEST_OK` on success, otherwise raises a fault
- **Why excluded from production:**
  - may increase boot time
  - may touch hardware in non-operational ways
  - increases code size and attack surface
  - production firmware should run the minimal verified path

## Repository map

- `main/system_cfg.*`      : frozen hardware config (const)
- `main/system_config.h`   : tuning (stack sizes, priorities, feature flags)
- `main/system_types.h`    : shared message types (no FreeRTOS, no drivers)
- `main/system_queues.*`   : global queues (init/deinit/dump)
- `main/system_state.*`    : global event groups + bits (init/deinit/dump)
- `main/system_faults.*`   : fault bitmap + rules (set/clear/query)
- `main/system_irq.*`      : ISR->task bridge helpers (ISR-safe patterns)
- `main/initialisation.*`  : boot orchestration (order of init/start)
- `main/module_dummy_*.*`  : example drivers (no FreeRTOS)
- `main/task_*.*`          : example tasks (ownership + logic)
- `main/API_*.*`           : example APIs (intent-level calls)
- `main/test_selftest.*`   : optional boot selftest

---

> *On se moque des fondations quand on n’a jamais eu à porter la maison.*
