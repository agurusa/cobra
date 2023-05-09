#include <string.h>

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_lighting_model_api.h"

#include "security_structs.h"
#include "provisioning.c"
#include "config_server.c"
#include "generic_onoff_server_model.c"
#include "generic_onoff_client_model.c"
#include "generic_user_property_server_model.c"
#include "light_hsl_server_model.c"
#include "common_server.c"

/***
 * Initialize the board's UUID. 
 * Setup callback methods for provisioning and configuration. 
 * Initialize the BLE Mesh module.
*/

const char * BLUETOOTH_TAG = "BLUETOOTH";

esp_err_t ble_mesh_init(esp_ble_mesh_elem_t *elements, size_t length_of_elements) {
    fill_composition_data(elements, length_of_elements, &composition);
    for (int i = 0; i < prop_server.property_count; i++){
        net_buf_simple_init(prop_server.properties[i].val, 0);
        prop_server.properties[i].val = 0x00;
    };
    esp_err_t err = ESP_OK;
    memcpy(dev_uuid + 2, esp_bt_dev_get_address(), 6);
    esp_ble_mesh_register_prov_callback(provisioning_callback);
    esp_ble_mesh_register_config_server_callback(config_server_callback);
    esp_ble_mesh_register_generic_server_callback(generic_server_cb);
    esp_ble_mesh_register_generic_client_callback(generic_onoff_client_cb);
    esp_ble_mesh_register_lighting_server_callback(hsl_server_cb);
    err = esp_ble_mesh_init(&provision, &composition);
    if (err != ESP_OK) {
        ESP_LOGE(BLUETOOTH_TAG, "Failed to initialize ble mesh (err %d)", err);
        return err;
    }

    err = esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);
    if (err != ESP_OK) {
        ESP_LOGE(BLUETOOTH_TAG, "Failed to enable mesh node (err %d)", err);
        return err;
    }


    ESP_LOGI(BLUETOOTH_TAG, "BLE Mesh Node initialized");
    return err;

}

/***
 * Initialize and enable the BT controller and bluedroid stack.
 * The BT controller implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL) and the Physical Layer (PHY). The BT Controller is invisible to the user applications and deals with the lower layers of the BLE stack. The controller configuration includes setting the BT controller stack size, priority and HCI baud rate. 
*/
esp_err_t bluetooth_init() {
    esp_err_t err = ESP_OK;
    esp_bt_controller_config_t config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&config);
    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    // initialize and enable the bluedroid stack, which includes the common definitions and APIs for both BT Classic and BLE
    err = esp_bluedroid_init();
    err = esp_bluedroid_enable();
    return err;
};