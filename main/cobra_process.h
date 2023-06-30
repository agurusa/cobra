#pragma once 

#include "state_enum.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_lighting_model_api.h"
#include "cobra_roles.h"

typedef enum {
    process_none, /* special process for queuing*/
    process_mode_button_press_long,
    process_mode_button_press_short,
    process_comms_button_press_long,
    process_comms_button_press_short,
    process_alarm_went_off,
    process_message_received,
    num_of_processes
}cobra_process_t;

typedef enum{
    none, /*special priority for queuing*/
    low,
    standard,
    high,
    num_of_priorities
}cobra_priority_t;

typedef struct {
    cobra_process_t process;
    cobra_priority_t priority;
    
}cobra_process_info_t;

typedef struct cobra_process_node_t{
    cobra_process_info_t info;
    struct cobra_process_node_t * next;
}cobra_process_node_t;


typedef struct {
    const int max_size;
    cobra_process_node_t * first;
    int size;
}cobra_process_queue_t;

typedef enum {
    message_acknowledged,
    message_silenced,
    message_snoozed,
    message_group_owner,
    message_location_requested,
    message_role_changed,
    message_usr_addr,
}cobra_bt_message_t;

typedef struct cobra_usr_t{
    uint16_t recv_addr;
    int recv_index;
}cobra_usr_t;

typedef union {
    cobra_role_t set_val_usr_role;
    cobra_colors_t set_val_comms_color;
    cobra_usr_t set_val_cobra_usr;
}ble_mesh_param_t;

typedef union{
    esp_ble_mesh_generic_server_cb_event_t server;
    esp_ble_mesh_generic_client_cb_event_t client;
    esp_ble_mesh_lighting_server_cb_event_t hsl_srv;
}ble_mesh_event_t;


typedef struct cobra_bt_response_t{
    cobra_bt_message_t response;
    struct cobra_bt_response_t * next;
    ble_mesh_event_t event;
    ble_mesh_param_t *param;
}cobra_bt_response_t;

typedef struct {
    cobra_bt_response_t * first;
}cobra_message_queue_t;