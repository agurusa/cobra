#pragma once 
#include "esp_ble_mesh_provisioning_api.h"
#include "security_structs.h"


const char * PROV_TAG = "PROVISIONING";

// fill the composition struct with all the elements
void fill_composition_data(esp_ble_mesh_elem_t * elements, size_t length_of_elements, esp_ble_mesh_comp_t * composition){
    composition->element_count = length_of_elements;
    composition->elements = elements;
}

// called when provisioning is complete
void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    ESP_LOGI(PROV_TAG, "provisioning complete");
    config_info.net_idx = net_idx;
    /* update_nvs_data() shall not be invoked here, because if the device
     * is restarted and goes into a provisioned state, then the following events
     * will come:
     * 1st: ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT
     * 2nd: ESP_BLE_MESH_PROV_REGISTER_COMP_EVT
     * So the config_info.net_idx will be updated here, and if we store the mesh 
     * info here, the wrong app_idx (initialized with 0xFFFF) will be stored in nvs
     * just before restoring it.
     */
}

// callback method for provisioning events. 
void provisioning_callback(esp_ble_mesh_prov_cb_event_t event,
                           esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ESP_LOGI(PROV_TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code %d", param->prov_register_comp.err_code);
        update_nvs_data(config_info); /* Restore proper mesh info */
        break;
    case ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(PROV_TAG, "ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT, err_code %d", param->node_prov_enable_comp.err_code);
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
        ESP_LOGI(PROV_TAG, "ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT, bearer %s",
            param->node_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(PROV_TAG, "ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT, bearer %s",
            param->node_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
        ESP_LOGI(PROV_TAG, "ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT");
        prov_complete(param->node_prov_complete.net_idx, param->node_prov_complete.addr,
            param->node_prov_complete.flags, param->node_prov_complete.iv_index);
        break;
    case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
        break;
    case ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT:
        ESP_LOGI(PROV_TAG, "ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT, err_code %d", param->node_set_unprov_dev_name_comp.err_code);
        break;
    default:
        break;
    }
}