#ifndef _GEN_ONOFF_CLIENT_MODEL_GUARD
#define _GEN_ONOFF_CLIENT_MODEL_GUARD

#include "esp_ble_mesh_generic_model_api.h"
#include "config_server.c"


const uint16_t GROUP_ADDR = 0xC000; /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
const char* GEN_ONOFF_TAG = "Gen_OnOff_Client"; /* logging*/

//************* GENERIC ONOFF CLIENT MODEL *************//
// root node: receives status from member bracelets. 
// transmits requests for get, set, and set unack of
// the body LEDs.
//***********************************************//
// Generic OnOff Client Model Conext
static esp_ble_mesh_client_t onoff_client;
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);

// send set on off message. acknowledged.
void send_gen_onoff_set(void) {
    esp_err_t err = ESP_OK;
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_generic_client_set_state_t set = {0};

    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET;
    common.model = onoff_client.model;
    common.ctx.net_idx  = config_info.net_idx;
    common.ctx.app_idx = config_info.app_idx;

    common.ctx.addr = GROUP_ADDR; /* send to all nodes subscribed to the group */
    common.ctx.send_ttl = 3; /* relay msg 3 times */
    common.ctx.send_rel = true;
    common.msg_timeout = 60000; /* ms */
    common.msg_role = ROLE_NODE;

    set.onoff_set.op_en = false;
    set.onoff_set.onoff = 1;
    set.onoff_set.tid = config_info.tid;
    config_info.tid++;

    err = esp_ble_mesh_generic_client_set_state(&common, &set); 
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

};

void generic_onoff_client_cb(esp_ble_mesh_generic_client_cb_event_t event, esp_ble_mesh_generic_client_cb_param_t *param)
{
        ESP_LOGI(GEN_ONOFF_TAG, "Generic client, event %u, error code %d, opcode is 0x%04x",
        event, param->error_code, param->params->opcode);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT");
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
            ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET, onoff %d", param->status_cb.onoff_status.present_onoff);
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT");
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
            ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET, onoff %d", param->status_cb.onoff_status.present_onoff);
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT");
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT");
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
            /* If failed to get the response of Generic OnOff Set, resend Generic OnOff Set  */
            // example_ble_mesh_send_gen_onoff_set();
        }
        break;
    default:
        ESP_LOGI(GEN_ONOFF_TAG, "unknown thing that's happening...");
        break;
    }

}

#endif