#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"

#include "esp_ble_mesh_common_api.h"

#include "nvs.c"
#include "health_server.c"
#include "bluetooth.c"
#include "config_server.c"
#include "generic_onoff_client_model.c"
#include "generic_onoff_server_model.c"

const char* TAG = "APP"; 
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
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

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


}