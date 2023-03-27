#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"

#include "esp_ble_mesh_common_api.h"
#include "esp_timer.h"

#include "state_enum.h"
#include "state_updater.c"
#include "cobra_button.h"
#include "cobra_leds.c"
#include "periodic_timer.c"
#include "nvs.c"
#include "health_server.c"
#include "bluetooth.c"
#include "config_server.c"
#include "generic_onoff_client_model.c"
#include "generic_onoff_server_model.c"

#define STACK_SIZE  2048

const char * TAG = "APP"; 
const uint16_t NO_DESCRIPTOR = 0; /* used for the Loc field in elements*/



esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
};

esp_ble_mesh_model_t secondary_models[] = {
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_serv_pub, &onoff_server),
};

// struct holding all elements
esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(NO_DESCRIPTOR, root_models, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(NO_DESCRIPTOR, secondary_models, ESP_BLE_MESH_MODEL_NONE),
};


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
    cobra_state_struct_t *state_info = (cobra_state_struct_t*)calloc(1, sizeof(cobra_state_struct_t)); /*currently used*/

    const esp_timer_create_args_t state_timer_args = {
        .callback = state_isr_callback,
        .arg = &state_info,
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
    /*debug only*/
    err = setupBoardLed();

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
    xTaskCreate(respond_to_state_change, "STATE_CHANGE_TASK", STACK_SIZE, &cobra_state, tskIDLE_PRIORITY, &state_update_handle);

    
}