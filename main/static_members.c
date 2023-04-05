#ifndef _STATIC_MEMBERS_GUARD
#define _STATIC_MEMBERS_GUARD

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_generic_model_api.h"

#include "cobra_process.h"
#include "config_struct.h"
#include "cobra_led_struct.h"

#define QUEUE_SIZE 10
#define STACK_SIZE  2048

static cobra_process_queue_t cobra_queue = {
    .max_size = QUEUE_SIZE,
    .first = NULL,
    .size = 0,
};

static cobra_message_queue_t rxn_messages = {
    .first = NULL,
};
static bool msg_received = false;

// updated with app keys after provisioning/configurations
static config_info_t config_info = {
    .net_idx = ESP_BLE_MESH_KEY_UNUSED,
    .app_idx = ESP_BLE_MESH_KEY_UNUSED,
    .tid = 0,
};

// Generic OnOff Client Model Conext
static esp_ble_mesh_client_t onoff_client;

// Generic OnOff Server Model Conext
static esp_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp =  ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.status_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP
};

//Blinks LED
static int led_on = 0;

/* For when we start a background task that blinks the LED */
static TaskHandle_t led_blink_handle = NULL;

#endif