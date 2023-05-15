#ifndef _GEN_USER_PROP_SERVER_MODEL_GUARD
#define _GEN_USER_PROP_SERVER_MODEL_GUARD

#include "esp_ble_mesh_generic_model_api.h"
#include "cobra_process.h"
#include "cobra_queue.c"
#include "ble_responses.c"
#include "static_members.c"

const char* GEN_PROP_SERVER_TAG = "Gen_Prop_Server"; /* logging*/

const esp_ble_mesh_generic_property_t usr_property_role = 
{
    .id = COBRA_ROLE_ID,
    .user_access = 0x03,
    .admin_access = 0x03,
    .manu_access = 0x03,
    .val = NET_BUF_SIMPLE(2)
};

esp_ble_mesh_generic_property_t properties[] = {
    usr_property_role
};

// Generic User Property Server Model Conext
static esp_ble_mesh_gen_user_prop_srv_t prop_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .rsp_ctrl.set_auto_rsp =  ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .rsp_ctrl.status_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .property_count = 1,
    .properties = properties,
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(property_serv_pub, 2+3, ROLE_NODE);


void handle_usr_prop_state_change(esp_ble_mesh_generic_server_cb_event_t event,
                            esp_ble_mesh_generic_server_cb_param_t *param){
    ESP_LOGE(GEN_PROP_SERVER_TAG, "usr prop state change");
    ESP_LOGE(GEN_PROP_SERVER_TAG, "changing the role. length is: 0x%04x", param->value.set.user_property.property_value->len);
    cobra_bt_response_t *msg = calloc(1, sizeof(cobra_bt_response_t));
    msg->response = message_from_phone_app;
    msg->next = NULL;
    msg->event.server = event;
    msg->param = calloc(1, sizeof(ble_mesh_param_t));
    msg->param->server = calloc(1, sizeof(esp_ble_mesh_generic_server_cb_param_t));
    *(msg->param->server) = *param;
    uint8_t * data = (uint8_t *)net_buf_simple_pull_mem(param->value.set.user_property.property_value, param->value.set.user_property.property_value->len);
    if (*data == 0x01){
        msg->param->set_val_usr_role = role_listener;
    }
    else if (*data == 0x00) {
        msg->param->set_val_usr_role = role_owner;
    }
    else {
        ESP_LOGE(BLE_QUEUE, "UNKNOWN ROLE RECEIVED!: 0x%02x", *data);
    }
    push_msg(msg);
    cobra_process_t proc = process_message_received;
    cobra_process_info_t proc_info = {
        .process = proc,
        .priority = high
    };
    add_process(proc_info);
};


#endif