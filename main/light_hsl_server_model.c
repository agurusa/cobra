#ifndef _LIGHT_HSL_SERVER_MODEL_GUARD
#define _LIGHT_HSL_SERVER_MODEL_GUARD
#include "esp_ble_mesh_lighting_model_api.h"
#include "ble_responses.c"
#include "cobra_colors.h"
#include "cobra_process.h"
#include "cobra_queue.c"

const char* HSL_SRV_TAG = "HSL_SERVER";

/* Light Lightness state related context */
static esp_ble_mesh_light_lightness_state_t lightness_state;

/* Light Lightness Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(lightness_pub, 2 + 5, ROLE_NODE);
static esp_ble_mesh_light_lightness_srv_t lightness_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .state = &lightness_state,
};

/* Light Lightness Setup Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(lightness_setup_pub, 2 + 5, ROLE_NODE);
static esp_ble_mesh_light_lightness_setup_srv_t lightness_setup_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .state = &lightness_state,
};

/* Light HSL state related context */
static esp_ble_mesh_light_hsl_state_t hsl_state;

/* Light HSL Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_srv_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_hsl_srv_t hsl_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,
};

/* Light HSL Setup Server related context */
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_setup_pub, 2 + 9, ROLE_NODE);
static esp_ble_mesh_light_hsl_setup_srv_t hsl_setup_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .state = &hsl_state,
};

void handle_hsl_set_message(esp_ble_mesh_lighting_server_cb_event_t event, cobra_colors_t color, cobra_bt_message_t msg_rsp)
{
    const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
    cobra_bt_response_t *msg = pvPortMalloc(sizeof(cobra_bt_response_t)); // passing by reference because cobra_bt_response_t is pretty large
    msg->response = msg_rsp;
    msg->event.hsl_srv = event;
    msg->param = pvPortMalloc(sizeof(ble_mesh_param_t));
    msg->param->set_val_comms_color = color;
    xQueueSend(bleMessageQueue, &msg, xBlockTime);
    cobra_process_t proc = process_message_received;
    cobra_process_info_t proc_info = {
        .process = proc,
        .priority = high
    };
    add_process(proc_info);

};

//TODO: why is saturation getting set as 0?? 
void hsl_server_cb(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    //TODO: saturation is always 0 here?? 
    ESP_LOGE(HSL_SRV_TAG,  "event: 0x%02x,  lightness: %u, hue: %u, saturation: %u", 
    event, 
    param->value.set.hsl.lightness,
    param->value.set.hsl.hue,
    param->value.set.hsl.saturation);
    cobra_colors_t color = {
        param->value.set.hsl.lightness,
        param->value.set.hsl.hue,
        100, //ugh. fix this when the above bug is figured out.
        param->ctx.addr
    };
    //if the color will be set to red, we know we have a silence message
    if (color.lightness == RED.lightness && color.hue == RED.hue){
        handle_hsl_set_message(event, color, message_silenced);
    }
    //if the color will be set to yellow, we received a snooze
    else if (color.lightness == YELLOW.lightness && color.hue == YELLOW.hue){
        handle_hsl_set_message(event, color, message_snoozed);
    }
    //if the color is anything else, we received an acknolwedge.
    else {
        handle_hsl_set_message(event, color, message_acknowledged);
    }
};

#endif