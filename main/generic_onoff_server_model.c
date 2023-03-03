#include "esp_ble_mesh_generic_model_api.h"

const char* GEN_ONOFF_SERVER_TAG = "Gen_OnOff_Server"; /* logging*/


//************* GENERIC ONOFF SERVER MODEL *************//
// root node: transmits status from member bracelets. 
// receives requests for get, set, and set unack of
// the body LEDs.
//***********************************************//
// Generic OnOff Server Model Conext

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_serv_pub, 2+3, ROLE_NODE);
static esp_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp =  ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.status_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP
};

/*TODO*/
void handle_generic_onoff_state_change(){
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "on off state change");
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

void generic_onff_server_cb (esp_ble_mesh_generic_server_cb_event_t event,
                            esp_ble_mesh_generic_server_cb_param_t *param){
    esp_ble_mesh_gen_onoff_srv_t *srv;
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);
    switch (event) {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        ESP_LOGI(GEN_ONOFF_SERVER_TAG, "ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT");
        ESP_LOGI(GEN_ONOFF_SERVER_TAG, "onoff 0x%02x", param->value.state_change.onoff_set.onoff);
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET){
            handle_generic_onoff_state_change();
        }
        else if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            handle_generic_onoff_state_change();
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