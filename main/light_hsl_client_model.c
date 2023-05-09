#ifndef _LIGHT_HSL_CLIENT_MODEL_GUARD
#define _LIGHT_HSL_CLIENT_MODEL_GUARD

#include "esp_ble_mesh_lighting_model_api.h"

#include "config_server.c"
#include "cobra_colors.h"
#include "static_members.c"

const char* HSL_CLIENT_TAG = "HSL_CLIENT";
static esp_ble_mesh_client_t hsl_client;
ESP_BLE_MESH_MODEL_PUB_DEFINE(hsl_cli_pub, 2 + 7, ROLE_NODE);

void send_hsl_set_message(cobra_colors_t color) {
    esp_err_t err = ESP_OK;
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_light_client_set_state_t set = {0};
    common.opcode = ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK;
    common.model = hsl_client.model;
    common.ctx.net_idx  = config_info.net_idx;
    common.ctx.app_idx = config_info.app_idx;

    common.ctx.addr = GROUP_ADDR; /* send to all nodes subscribed to the group */
    common.ctx.send_ttl = 3; /* relay msg 3 times */
    common.ctx.send_rel = true;
    common.msg_timeout = 60000; /* ms */
    common.msg_role = ROLE_NODE;

    set.hsl_set.op_en = false;
    set.hsl_set.hsl_lightness = color.lightness;
    set.hsl_set.hsl_hue = color.hue;
    set.hsl_set.hsl_saturation = color.saturation;
    set.hsl_set.tid = config_info.tid;
    config_info.tid++;
    err = esp_ble_mesh_light_client_set_state(&common, &set);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
};

#endif