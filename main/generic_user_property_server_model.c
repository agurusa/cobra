#ifndef _GEN_USER_PROP_SERVER_MODEL_GUARD
#define _GEN_USER_PROP_SERVER_MODEL_GUARD

#include "esp_ble_mesh_generic_model_api.h"
#include "esp_check.h"

#include "cobra_process.h"
#include "cobra_queue.c"
#include "ble_responses.c"
#include "static_members.h"

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
    ESP_LOGI(GEN_PROP_SERVER_TAG, "usr prop state change");
    cobra_bt_response_t *msg = calloc(1, sizeof(cobra_bt_response_t));
    msg->next = NULL;
    msg->event.server = event;
    msg->param = calloc(1, sizeof(ble_mesh_param_t));

    uint16_t property_id = param->value.set.user_property.property_id;
    uint16_t len = param->value.set.user_property.property_value->len;
    uint8_t * data = (uint8_t *)net_buf_simple_pull_mem(param->value.set.user_property.property_value, len);
    ESP_LOGI(GEN_PROP_SERVER_TAG, "PROPERTY ID: 0x%04x, DATA LENGTH: %u, DATA: 0x%02x", property_id, len, *data);
    if(property_id == 0x1111){
        msg->response = message_role_changed;
        if (*data == 0x01){
            msg->param->set_val_usr_role = role_listener;
        }
        else if (*data == 0x00) {
            msg->param->set_val_usr_role = role_owner;
        }
        else {
            ESP_LOGE(GEN_PROP_SERVER_TAG, "unknown cobra role rcvd: %u", *data);
            free(msg->param);
            free(msg);
            return;
        }
    }
    //this is a message about the members of the group
    else if(property_id == 0x1112) {
        if(len != 3){
            ESP_LOGE(GEN_PROP_SERVER_TAG, "didnt receive a packet of length 3 for property id 0x1112.");
            free(msg->param);
            free(msg);
            return;
        }
        msg->response = message_usr_addr;
        cobra_usr_t usr = {
            ((uint16_t)data[0] << 8) | data[1],
            data[2]
        };
        msg->param->set_val_cobra_usr = usr;
    }
    else {
        ESP_LOGE(GEN_PROP_SERVER_TAG, "unknown user property rcvd. prop id: 0x%04x", property_id);
        free(msg->param);
        free(msg);
        return;
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