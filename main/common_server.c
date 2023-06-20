#ifndef _COMMON_SERVER_HEADER
#define _COMMON_SERVER_HEADER

#include "generic_onoff_server_model.c"
#include "generic_user_property_server_model.c"

const char* COMMON_SERV_TAG = "COMMON_SERVER";

void generic_server_cb (esp_ble_mesh_generic_server_cb_event_t event,
                            esp_ble_mesh_generic_server_cb_param_t *param){
    ESP_LOGI("RECEIVED_MSG_EVENT", "event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x",
        event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);
    uint32_t opcode = param->ctx.recv_op;
    switch (event) {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        ESP_LOGI(COMMON_SERV_TAG, "STATE CHANGE EVENT");
        if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET || 
            opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK){
            handle_generic_onoff_state_change(event, param);
        }
        else if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK || 
                 opcode == ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET){
                handle_usr_prop_state_change(event, param);
                }
        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
        ESP_LOGI(COMMON_SERV_TAG, "GET EVENT");
        //TODO
        break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
        ESP_LOGI(COMMON_SERV_TAG, "SET EVENT");
        if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET || 
            opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK){
            handle_generic_onoff_state_change(event, param);
        }
        else if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK ||       
                 opcode == ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET){
                handle_usr_prop_state_change(event, param);
                }
        break;
    default:
        ESP_LOGE(COMMON_SERV_TAG, "Unknown Generic Server event 0x%02x", event);
        break;
    }

}

#endif