#include <stdio.h>

#include "esp_ble_mesh_health_model_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"

//************* HEALTH SERVER MODEL *************//
// mandatory: node diagnostics
//***********************************************//
// ESP BLE Mesh Health Server Model Conext
uint8_t test_ids[1] = {0x00};
ESP_BLE_MESH_MODEL_PUB_DEFINE(health_pub, 2 + 11, ROLE_NODE);
static esp_ble_mesh_health_srv_t health_server = {
    .health_test.id_count = 1,
    .health_test.test_ids = test_ids,
};

//************* CONFIGURATION SERVER MODEL *************//
// mandatory: represents mesh network configurations of a device
//***********************************************//
// ESP BLE Configuration Server Model Conext
static esp_ble_mesh_cfg_srv_t config_server = {
    .relay = ESP_BLE_MESH_RELAY_ENABLED,
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    /* 3 transmissions with 20ms interval */
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
};

//************* GENERIC_ONOFF SERVER MODEL *************//
// root model: supports pub/sub 
// must support
// rx: 
// onoff get
// onoff set
// onoff set unacknoweldged
// tx:
// onoff status
//***********************************************//
// ONOFF SERVER model context
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub_0, 2 + 3, ROLE_NODE); 
static esp_ble_mesh_gen_onoff_srv_t onoff_server_0 = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP, // response of Client Get messages will be replied by the server models
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP, // response of Client Set messages will be replied by the server models
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub_0, &onoff_server_0),
};




void app_main(void)
{

}