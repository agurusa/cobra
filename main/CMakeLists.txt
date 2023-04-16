idf_component_register(SRCS "state_updater.c" "cobra_leds.c" "light_hsl_server_model.c" "light_hsl_client_model.c" "periodic_timer.c" "provisioning.c" "config_server.c" "health_server.c" "nvs.c" "bluetooth.c" "generic_onoff_client_model.c" "generic_onoff_server_model.c" "main.c"
                    INCLUDE_DIRS ".")

component_compile_options(-Wno-error=format= -Wno-format)