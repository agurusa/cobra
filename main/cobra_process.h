#pragma once 

#include "state_enum.h"

typedef enum {
    process_none, /* special process for queuing*/
    process_mode_button_press_long,
    process_mode_button_press_short,
    process_comms_button_press_long,
    process_comms_button_press_short,
    process_alarm_went_off,
    process_message_received
}cobra_process_t;

typedef enum{
    none, /*special priority for queuing*/
    low,
    standard,
    high,
}cobra_priority_t;

typedef struct {
    cobra_process_t process;
    cobra_priority_t priority;
    cobra_state_struct_t cobra_state; /* TODO: get rid of this? */
    
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