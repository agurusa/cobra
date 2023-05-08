#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"

#include "esp_ble_mesh_common_api.h"
#include "esp_timer.h"

#include "state_enum.h"
#include "state_updater.c"
#include "cobra_button.h"
#include "cobra_colors.h"
#include "cobra_leds.c"
#include "periodic_timer.c"
#include "nvs.c"
#include "health_server.c"
#include "bluetooth.c"
#include "config_server.c"
#include "generic_onoff_client_model.c"
#include "generic_onoff_server_model.c"
#include "light_hsl_client_model.c"
#include "light_hsl_server_model.c"
#include "generic_user_property_server_model.c"
#include "static_members.c"

/*TODO: update these based on what is gathered by the app sync*/
#define COBRA_ROLE role_listener
#define USER_LIGHTNESS 20
#define USER_HUE 20
#define USER_SATURATION 20

const char * TAG = "APP"; 
const uint16_t NO_DESCRIPTOR = 0; /* used for the Loc field in elements*/


esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
};

esp_ble_mesh_model_t secondary_models[] = {
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_serv_pub, &onoff_server),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_CLI(&hsl_cli_pub, &hsl_client),
    ESP_BLE_MESH_MODEL_GEN_USER_PROP_SRV(&property_serv_pub, &prop_server)
};

esp_ble_mesh_model_t light_hsl_models[] = {
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&lightness_pub, &lightness_server),
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(&lightness_setup_pub, &lightness_setup_server),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SRV(&hsl_srv_pub, &hsl_server),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV(&hsl_setup_pub, &hsl_setup_server),
};

// struct holding all elements
esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(NO_DESCRIPTOR, root_models, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(NO_DESCRIPTOR, secondary_models, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(NO_DESCRIPTOR, light_hsl_models, ESP_BLE_MESH_MODEL_NONE)
};

void set_initial_state(cobra_state_struct_t *state, cobra_colors_t *user_color)
{
    (*state).current_state = state_startup;
    (*state).next_state = state_startup;
    (*state).group_role = COBRA_ROLE;
    (*state).current_mode = mode_music;
    (*user_color).lightness = USER_LIGHTNESS;
    (*user_color).hue = USER_HUE;
    (*user_color).saturation = USER_SATURATION;
    (*state).user_color = *user_color;
};

void print_state(cobra_state_struct_t *state)
{
    ESP_LOGE(TAG, "current state: %i, next state: %i, role %i, current mode %i, lightness: %u, hue %u, saturation %u", (*state).current_state, (*state).next_state, (*state).group_role, (*state).current_mode, (*state).user_color.lightness, (*state).user_color.hue, (*state).user_color.saturation);
}

void app_main(void)
{   
    
    esp_err_t err = ESP_OK;

    /* BUTTON TIMER */
    timer_info_t *timer_info = (timer_info_t*)calloc(1, sizeof(timer_info_t));

    const esp_timer_create_args_t periodic_timer_args = {
    .callback = timer_isr_callback,
    .arg = &timer_info,
    .name = "periodic_button"
    };

    esp_timer_handle_t periodic_timer;
    err = esp_timer_create(&periodic_timer_args, &periodic_timer);
    ESP_ERROR_CHECK(err);

    /* STATE TIMER */
    cobra_state_struct_t *cobra_state = calloc(1, sizeof(cobra_state_struct_t));
     cobra_colors_t *user_color = calloc(1, sizeof(cobra_colors_t));
    set_initial_state(cobra_state, user_color);
    print_state(cobra_state);

    const esp_timer_create_args_t state_timer_args = {
        .callback = state_isr_callback,
        .arg = cobra_state,
        .name = "state_timer"
    };

    esp_timer_handle_t state_timer;
    err = esp_timer_create(&state_timer_args, &state_timer);
    ESP_ERROR_CHECK(err);


    /* HARDWARE SETUP */
    err = button_setup(COMMS_BUTTON_PIN);
    err = button_setup(MODE_BUTTON_PIN);
    ESP_ERROR_CHECK(err);
    err = setupBodyLeds();
    ESP_ERROR_CHECK(err);
    /*debug only*/
    // err = setupBoardLed();

    /* START TIMERS */
    err = esp_timer_start_periodic(periodic_timer, PERIOD_MS);
    err = esp_timer_start_periodic(state_timer, PERIOD_MS*100000);
    ESP_ERROR_CHECK(err);

    /* START NVS */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* BLUETOOTH AND BLE SETUP*/
    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }
    ESP_ERROR_CHECK(err);

    /* Open nvs namespace for storing/restoring mesh example info */
    err = ble_mesh_nvs_open(&NVS_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "nvs open failed (err %d)", err);
        return;
    }
    ESP_ERROR_CHECK(err);


    err = ble_mesh_init(elements, sizeof(elements)/sizeof(elements[0]));
    

    if (err) {
        ESP_LOGE(TAG, "ble mesh init failed (err %d)", err);
        return;
    }
    ESP_ERROR_CHECK(err);


    /* Start background task that checks the state of the buttons */
    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;
    xTaskCreate(check_buttons, "CHECK_BUTTON_TASK", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle);

    /* Start a background task to respond to a state change */
    TaskHandle_t state_update_handle = NULL;
    xTaskCreate(respond_to_state_change, "STATE_CHANGE_TASK", STACK_SIZE, cobra_state, tskIDLE_PRIORITY, &state_update_handle);

    /* Start a background task that processes nodes from the queue */
    TaskHandle_t queue_handle = NULL;
    xTaskCreate(pop_process, "PROCESS_QUEUE_TASK", STACK_SIZE, cobra_state, tskIDLE_PRIORITY, &queue_handle);
    
}