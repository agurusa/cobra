#ifndef _ONOFF_SERVER_GUARD
#define _ONOFF_SERVER_GUARD
#include "esp_ble_mesh_generic_model_api.h"
#include "cobra_process.h"
#include "cobra_queue.c"
#include "ble_responses.c"
#include "static_members.c"

const char* GEN_ONOFF_SERVER_TAG = "Gen_OnOff_Server"; /* logging*/


//************* GENERIC ONOFF SERVER MODEL *************//
// root node: transmits status from member bracelets. 
// receives requests for get, set, and set unack of
// the body LEDs.
//***********************************************//

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_serv_pub, 2+3, ROLE_NODE);

/*TODO*/
void handle_generic_onoff_state_change(esp_ble_mesh_generic_server_cb_event_t event,
                            esp_ble_mesh_generic_server_cb_param_t *param){
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "on off state change");
    cobra_bt_response_t *msg = calloc(1, sizeof(cobra_bt_response_t));
    msg->response = message_group_owner;
    msg->next = NULL;
    msg->event.server = event;
    msg->param = calloc(1, sizeof(ble_mesh_param_t));
    msg->param->server = calloc(1, sizeof(esp_ble_mesh_generic_server_cb_param_t));
    *(msg->param->server) = *param;
    push_msg(msg);
    cobra_process_t proc = process_message_received;
    cobra_process_info_t proc_info = {
        .process = proc,
        .priority = high
    };
    add_process(proc_info);
};

/*TODO*/
void handle_generic_onoff_get(){
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ON OFF GET");
};

/*TODO*/
void handle_generic_onoff_set(){
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ON OFF SET");
};

/*TODO*/
void handle_generic_onoff_set_unack(){
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ONOFF SET UNACK");
};

void generic_onoff_server_cb (esp_ble_mesh_generic_server_cb_event_t event,
                            esp_ble_mesh_generic_server_cb_param_t *param){
    esp_ble_mesh_gen_onoff_srv_t *srv;
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);
    switch (event) {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        /*TODO: figure out why we're here and not ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT*/
        ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT");
        ESP_LOGI(GEN_ONOFF_SERVER_TAG, "onoff 0x%02x", param->value.state_change.onoff_set.onoff);
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET){
            handle_generic_onoff_state_change(event, param);
        }
        else if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            handle_generic_onoff_state_change(event, param);
        }
        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
        ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
            srv = param->model->user_data;
            ESP_LOGI(GEN_ONOFF_SERVER_TAG, "onoff 0x%02x", srv->state.onoff);
            handle_generic_onoff_get();
        }
        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
        ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
            param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            ESP_LOGI(GEN_ONOFF_SERVER_TAG, "onoff 0x%02x, tid 0x%02x", param->value.set.onoff.onoff, param->value.set.onoff.tid);
            if (param->value.set.onoff.op_en) {
                ESP_LOGI(GEN_ONOFF_SERVER_TAG, "trans_time 0x%02x, delay 0x%02x",
                    param->value.set.onoff.trans_time, param->value.set.onoff.delay);
            }
            handle_generic_onoff_set();
        }
        break;
    default:
        ESP_LOGE(GEN_ONOFF_SERVER_TAG, "Unknown Generic Server event 0x%02x", event);
        break;
    }

}

#endif