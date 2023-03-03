#pragma once

#include "esp_log.h"
#include "nvs_flash.h"

#include "config_struct.h"

nvs_handle_t NVS_HANDLE; /* Used to store app keys */
const char * NVS_NAME = "cobra";
const char * NVS_KEY = "cobra";
const char * NVS_TAG = "NVS";


// stores networking info in flash Non Volatile Memory to save security keys even after power cycling
esp_err_t ble_mesh_nvs_store(nvs_handle_t handle, const char *key, const void *data, size_t length)
{
    esp_err_t err = ESP_OK;

    if (key == NULL || data == NULL || length == 0) {
        ESP_LOGE(NVS_TAG, "Store, invalid parameter");
        return ESP_ERR_INVALID_ARG;
    }

    err = nvs_set_blob(handle, key, data, length);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Store, nvs_set_blob failed, err %d", err);
        return err;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Store, nvs_commit failed, err %d", err);
        return err;
    }

    ESP_LOGI(NVS_TAG, "Store, key \"%s\", length %u", key, length);
    ESP_LOG_BUFFER_HEX("EXAMPLE_NVS: Store, data", data, length);
    return err;
}

esp_err_t ble_mesh_nvs_open(nvs_handle_t *handle)
{
    esp_err_t err = ESP_OK;

    if (handle == NULL) {
        ESP_LOGE(NVS_TAG, "Open, invalid nvs handle");
        return ESP_ERR_INVALID_ARG;
    }

    err = nvs_open(NVS_NAME, NVS_READWRITE, handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Open, nvs_open failed, err %d", err);
        return err;
    }

    ESP_LOGI(NVS_TAG, "Open namespace done, name \"%s\"", NVS_NAME);
    return err;
}

void update_nvs_data(config_info_t config_info) {
    ble_mesh_nvs_store(NVS_HANDLE, NVS_KEY, &config_info, sizeof(config_info));
}