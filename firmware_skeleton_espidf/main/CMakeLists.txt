idf_component_register(
    SRCS
        "app_main.c"
        "initialisation.c"
        "system_cfg.c"
        "system_queues.c"
        "system_state.c"
        "system_faults.c"
        "system_irq.c"
        "test_selftest.c"
        "module_dummy_sensor.c"
        "module_dummy_radio.c"
        "task_measure.c"
        "task_radio.c"
        "API_measure.c"
        "API_radio.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_timer
)
