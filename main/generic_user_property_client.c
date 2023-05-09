#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "config_server.c"
#include "static_members.c"


const char* GEN_USR_PROP_CLI_TAG = "PROP_CLI";
static esp_ble_mesh_client_t gen_prop_cli;

ESP_BLE_MESH_MODEL_PUB_DEFINE(gen_prop_cli_pub, 2 + 3, ROLE_NODE);

void send_gen_prop_cli_get(void){
    esp_err_t err = ESP_OK;
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_generic_client_get_state_t set = {0};

    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET;
    common.model = gen_prop_cli.model;
    common.ctx.net_idx  = config_info.net_idx;
    common.ctx.app_idx = config_info.app_idx;

    common.ctx.addr = GROUP_ADDR; /* send to all nodes subscribed to the group */
    common.ctx.send_ttl = 3; /* relay msg 3 times */
    common.ctx.send_rel = true;
    common.msg_timeout = 60000; /* ms */
    common.msg_role = ROLE_NODE;

    set.user_property_get.property_id = 0x1111;
    config_info.tid++;
    err = esp_ble_mesh_generic_client_get_state(&common, &set); 
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    ESP_LOGE(GEN_USR_PROP_CLI_TAG, "error: %d", err);

}

void usr_prop_cli_cb(esp_ble_mesh_generic_client_cb_event_t event, esp_ble_mesh_generic_client_cb_param_t *param)
{
        ESP_LOGI(GEN_USR_PROP_CLI_TAG, "Generic client, event %u, error code %d, opcode is 0x%04x",
        event, param->error_code, param->params->opcode);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        ESP_LOGI(GEN_USR_PROP_CLI_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT");

        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET) {
            ESP_LOGI(GEN_USR_PROP_CLI_TAG, "ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET");
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        ESP_LOGI(GEN_USR_PROP_CLI_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT");
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        ESP_LOGI(GEN_USR_PROP_CLI_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT");
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        ESP_LOGI(GEN_USR_PROP_CLI_TAG, "ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT");
        break;
    default:
        ESP_LOGI(GEN_USR_PROP_CLI_TAG, "unknown thing that's happening...");
        break;
    }

}