idf_component_register(SRCS "periodic_timer.c" "provisioning.c" "config_server.c" "health_server.c" "nvs.c" "bluetooth.c" "generic_onoff_client_model.c" "generic_onoff_server_model.c" "main.c"
                    INCLUDE_DIRS ".")

component_compile_options(-Wno-error=format= -Wno-format)