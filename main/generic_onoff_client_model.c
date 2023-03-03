#pragma once

#include "esp_ble_mesh_generic_model_api.h"


const uint16_t GROUP_ADDR = 0xC000; /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
const char* GEN_ONOFF_TAG = "Gen_OnOff_Client"; /* logging*/

//************* GENERIC ONOFF CLIENT MODEL *************//
// root node: receives status from member bracelets. 
// transmits requests for get, set, and set unack of
// the body LEDs.
//***********************************************//
// Generic OnOff Client Model Conext
esp_ble_mesh_client_t onoff_client;
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, ROLE_NODE);

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