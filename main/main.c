#include <stdio.h>

#include "nvs_flash.h"

#include "esp_ble_mesh_health_model_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"

const uint16_t GROUP_ADDR = 0xC000; /* Group Address assigned to all Group Members */
const uint16_t NOT_VENDOR_MODEL = 0xFFF; /* See ESP-IDF API reference for company ID */
nvs_handle_t NVS_HANDLE; /* Used to store app keys */
const char * NVS_KEY = "onoff_client";
static const char* TAG = "Gen_OnOff_Client"; /* logging*/

typedef struct {
    uint16_t net_idx; /* NetKey Index */
    uint16_t app_idx; /* AppKey Index */
}config_info_t;

// updated with app keys after provisioning/configurations
config_info_t config_info = {
    .net_idx = ESP_BLE_MESH_KEY_UNUSED,
    .app_idx = ESP_BLE_MESH_KEY_UNUSED,
};



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

//************* GENERIC ONOFF CLIENT MODEL *************//
// root node: receives status from member bracelets. 
// transmits requests for get, set, and set unack of
// the body LEDs.
//***********************************************//
// Generic OnOff Client Model Conext
static esp_ble_mesh_client_t onoff_client;
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);


static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
};

// stores networking info in flash Non Volatile Memory to save security keys even after power cycling
esp_err_t ble_mesh_nvs_store(nvs_handle_t handle, const char *key, const void *data, size_t length)
{
    esp_err_t err = ESP_OK;

    if (key == NULL || data == NULL || length == 0) {
        ESP_LOGE(TAG, "Store, invalid parameter");
        return ESP_ERR_INVALID_ARG;
    }

    err = nvs_set_blob(handle, key, data, length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Store, nvs_set_blob failed, err %d", err);
        return err;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Store, nvs_commit failed, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Store, key \"%s\", length %u", key, length);
    ESP_LOG_BUFFER_HEX("EXAMPLE_NVS: Store, data", data, length);
    return err;
}

void update_nvs_data() {
    ble_mesh_nvs_store(NVS_HANDLE, NVS_KEY, &config_info, sizeof(config_info));
}

// send set on off message. acknowledged.
void send_gen_onoff_set(void) {
    esp_err_t err = ESP_OK;
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_generic_client_set_state_t set = {0};

    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET;
    common.model = onoff_client.model;
    // common.ctx.net_idx  = TODO
    // common.ctx.app_idx = TODO
    common.ctx.addr = GROUP_ADDR; /* send to all nodes subscribed to the group */
    common.ctx.send_ttl = 3; /* relay msg 3 times */
    common.ctx.send_rel = true;
    common.msg_timeout = 60000; /* ms */
    common.msg_role = ROLE_NODE;

    set.onoff_set.op_en = false;
    // set.onoff_set.onoff = TODO
    // set.onoff_set.tid = TODO

    err = esp_ble_mesh_generic_client_set_state(&common, &set);


    

};

// callback method for when the configuration server receives configuration info
// from the provisioning client.
static void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
                                              esp_ble_mesh_cfg_server_cb_param_t *param)
{
    if (event == ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT) {
        switch (param->ctx.recv_op) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD");
            ESP_LOGI(TAG, "net_idx 0x%04x, app_idx 0x%04x",
                param->value.state_change.appkey_add.net_idx,
                param->value.state_change.appkey_add.app_idx);
            ESP_LOG_BUFFER_HEX("AppKey", param->value.state_change.appkey_add.app_key, 16);
            break;
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND");
            ESP_LOGI(TAG, "elem_addr 0x%04x, app_idx 0x%04x, cid 0x%04x, mod_id 0x%04x",
                param->value.state_change.mod_app_bind.element_addr,
                param->value.state_change.mod_app_bind.app_idx,
                param->value.state_change.mod_app_bind.company_id,
                param->value.state_change.mod_app_bind.model_id);
            if (param->value.state_change.mod_app_bind.company_id == NOT_VENDOR_MODEL &&
                param->value.state_change.mod_app_bind.model_id == ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI) {
                config_info.app_idx = param->value.state_change.mod_app_bind.app_idx;
                update_nvs_data(); /* Store proper mesh example info */
            }
            break;
        default:
            break;
        }
    }
}



void app_main(void)
{

}