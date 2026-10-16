add_executable(exe2
        main.c
)

target_link_libraries(exe1 pico_stdlib hardware_adc freertos)
pico_add_extra_outputs(exe1)
