#ifndef _ONOFF_SERVER_GUARD
#define _ONOFF_SERVER_GUARD
#include "esp_ble_mesh_generic_model_api.h"
#include "cobra_process.h"
#include "cobra_queue.c"
#include "ble_responses.c"
#include "static_members.h"

const char* GEN_ONOFF_SERVER_TAG = "Gen_OnOff_Server"; /* logging*/

// Generic OnOff Server Model Conext
static esp_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp =  ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.status_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP
};

//************* GENERIC ONOFF SERVER MODEL *************//
// root node: transmits status from member bracelets. 
// receives requests for get, set, and set unack of
// the body LEDs.
//***********************************************//

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_serv_pub, 2+3, ROLE_NODE);

void handle_generic_onoff_state_change(esp_ble_mesh_generic_server_cb_event_t event,
                            esp_ble_mesh_generic_server_cb_param_t *param){
    ESP_LOGI(GEN_ONOFF_SERVER_TAG, "on off state change");
    cobra_bt_response_t *msg = calloc(1, sizeof(cobra_bt_response_t));
    msg->response = message_group_owner;
    msg->next = NULL;
    msg->event.server = event;
    msg->param = calloc(1, sizeof(ble_mesh_param_t));
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


#endif