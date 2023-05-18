#ifndef _GEN_ONOFF_CLIENT_MODEL_GUARD
#define _GEN_ONOFF_CLIENT_MODEL_GUARD

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "config_server.c"
#include "cobra_process.h"
#include "ble_responses.c"
#include "static_members.c"

const char* GEN_ONOFF_TAG = "Gen_OnOff_Client"; /* logging*/
// Generic OnOff Client Model Conext
static esp_ble_mesh_client_t onoff_client;

//************* GENERIC ONOFF CLIENT MODEL *************//
// root node: receives status from member bracelets. 
// transmits requests for get, set, and set unack of
// the body LEDs.
//***********************************************//

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);

/*debug*/
void log_ble_mesh_client_params(esp_ble_mesh_client_common_param_t *client_params,
                                esp_ble_mesh_generic_client_set_state_t *set_state)
{
    ESP_LOGE("BLE_MESH_CLIENT", "Client Model ID: 0x%04X, Client context net idx: 0x%04X, Client context app index: 0x%04X, Opcode: 0x%04X, TID: %i, onoff: %i",
             client_params->model->model_id, client_params->ctx.net_idx, client_params->ctx.app_idx, 
             client_params->opcode, set_state->onoff_set.tid, set_state->onoff_set.onoff);
}
 

// send set on off message. acknowledged.
void send_gen_onoff_set(void) {
    esp_err_t err = ESP_OK;
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_generic_client_set_state_t set = {0};

    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK;
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

    log_ble_mesh_client_params(&common, &set);
    err = esp_ble_mesh_generic_client_set_state(&common, &set); 
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

};

void generic_onoff_client_cb(esp_ble_mesh_generic_client_cb_event_t event, esp_ble_mesh_generic_client_cb_param_t *param)
{
        ESP_LOGI(GEN_ONOFF_TAG, "Generic client, event %u, error code %d, opcode is 0x%04x",
        event, param->error_code, param->params->opcode);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        /*TODO: figure out why we're not hitting ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT. potentially the payload is wrong*/
        ESP_LOGI(GEN_ONOFF_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT");
        cobra_process_t proc = process_message_received;

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