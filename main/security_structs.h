#pragma once

const uint16_t CID_ESPRESSIF = 0x02E5;
uint8_t dev_uuid[16] = { 0xdd, 0xdd };

// struct holding provisioning info
esp_ble_mesh_prov_t provision = {
    .uuid = dev_uuid,
    .output_size = 0,
    .output_actions = ESP_BLE_MESH_NO_OUTPUT, /* output not supported by device. todo: maybe device should blink? ESP_BLE_MESH_BLINK */
    .input_actions = ESP_BLE_MESH_NO_INPUT, /* input not supported by device. todo: maybe should push button on device? ESP_BLE_MESH_PUSH */
    .input_size = 0,
};

// struct holding composition data. Filled after elements are instantiated in main.
esp_ble_mesh_comp_t composition = {
    .cid = CID_ESPRESSIF,
    .element_count = 0,
    .elements = NULL,
};