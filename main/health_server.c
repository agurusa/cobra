#pragma once 
#include "esp_ble_mesh_health_model_api.h"


//************* HEALTH SERVER MODEL *************//
// mandatory: node diagnostics
//***********************************************//
// ESP BLE Mesh Health Server Model Conext
uint8_t test_ids[1] = {0x00};
ESP_BLE_MESH_MODEL_PUB_DEFINE(health_pub, 2 + 11, ROLE_NODE);
esp_ble_mesh_health_srv_t health_server = {
    .health_test.id_count = 1,
    .health_test.test_ids = test_ids,
};