#ifndef _LIGHT_HSL_SERVER_MODEL_GUARD
#define _LIGHT_HSL_SERVER_MODEL_GUARD
#include "esp_ble_mesh_lighting_model_api.h"

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

void handle_hsl_set_message()
{

};

void hsl_server_cb(esp_ble_mesh_lighting_server_state_change_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{

};

#endif