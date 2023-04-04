#ifndef _CONFIG_SERVER_GUARD
#define _CONFIG_SERVER_GUARD

#include "esp_ble_mesh_config_model_api.h"
#include "config_struct.h"
#include "nvs.c"

const uint16_t NOT_VENDOR_MODEL = 0xFFFF; /* See ESP-IDF API reference for company ID */
const char* SERVER_TAG = "CONFIG_SERVER"; /* logging*/

// updated with app keys after provisioning/configurations
static config_info_t config_info = {
    .net_idx = ESP_BLE_MESH_KEY_UNUSED,
    .app_idx = ESP_BLE_MESH_KEY_UNUSED,
    .tid = 0,
};


//************* CONFIGURATION SERVER MODEL *************//
// mandatory: represents mesh network configurations of a device
//***********************************************//
// ESP BLE Configuration Server Model Conext
esp_ble_mesh_cfg_srv_t config_server = {
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

// callback method for when the configuration server receives configuration info
// from the configuration client.
void config_server_callback(esp_ble_mesh_cfg_server_cb_event_t event,
                            esp_ble_mesh_cfg_server_cb_param_t *param)
{
    if (event == ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT) {
        switch (param->ctx.recv_op) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            ESP_LOGI(SERVER_TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD");
            ESP_LOGI(SERVER_TAG, "net_idx 0x%04x, app_idx 0x%04x",
                param->value.state_change.appkey_add.net_idx,
                param->value.state_change.appkey_add.app_idx);
            ESP_LOG_BUFFER_HEX("AppKey", param->value.state_change.appkey_add.app_key, 16);
            break;
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            ESP_LOGI(SERVER_TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND");
            ESP_LOGI(SERVER_TAG, "elem_addr 0x%04x, app_idx 0x%04x, cid 0x%04x, mod_id 0x%04x",
                param->value.state_change.mod_app_bind.element_addr,
                param->value.state_change.mod_app_bind.app_idx,
                param->value.state_change.mod_app_bind.company_id,
                param->value.state_change.mod_app_bind.model_id);
            if (param->value.state_change.mod_app_bind.company_id == NOT_VENDOR_MODEL){ 
                config_info.app_idx = param->value.state_change.mod_app_bind.app_idx;
                ESP_LOGE(SERVER_TAG, "saving app_idx: 0x%04x",param->value.state_change.mod_app_bind.app_idx );
                update_nvs_data(config_info); /* Store proper mesh example info */
            }
            break;
        default:
            break;
        }
    }
};

#endif