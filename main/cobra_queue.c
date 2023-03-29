#ifndef _COBRA_QUEUE_GUARD
#define _COBRA_QUEUE_GUARD

#define QUEUE_SIZE 10

#include <string.h>

#include "cobra_process.h"
#include "cobra_errors.h"
#include "mode_button.c"
#include "comms_button.c"

const char * QUEUE_TAG = "QUEUE";

static cobra_process_queue_t cobra_queue = {
    .max_size = QUEUE_SIZE,
    .first = NULL,
    .size = 0,
};

/* return the node where this should be placed AFTER in the queue. IE, how quickly it should be processed. 
Assumes that the first node is NOT null.
*/
cobra_process_node_t * get_node(cobra_priority_t priority) {
    cobra_process_node_t * node =  cobra_queue.first;
    while (node->next!=NULL)
    {
        if(node->next->info.priority == priority - 1)
        {
            return node;
        }
        node = node->next;

    }
    return node;
};

void print_queue(){
    cobra_process_node_t *node = cobra_queue.first;
    while(node!=NULL)
    {
        ESP_LOGE(QUEUE_TAG, "%i, %i", node->info.process, node->info.priority);
        node = node->next;
    }

}

// TODO: implement cobra errors that say whether or not the queue is full
void add_process(cobra_process_info_t info)
{
    if (cobra_queue.size == cobra_queue.max_size)
    {
        ESP_LOGE(QUEUE_TAG, "QUEUE FULL!");
        return;
    }

    cobra_process_node_t *new_node = (cobra_process_node_t*)calloc(1, sizeof(cobra_process_node_t));
    new_node->info = (cobra_process_info_t){
        .process = info.process,
        .priority = info.priority,
    };
    if (cobra_queue.size == 0)
    {
        cobra_queue.first = new_node;
    }
    else
    {
        cobra_process_node_t * node = get_node(info.priority);
        cobra_process_node_t * temp = node->next;
        node->next = new_node;
        new_node->next = temp;

    }
    cobra_queue.size++;
    print_queue();
}

void process_node(cobra_process_node_t node, cobra_state_struct_t *cobra_state)
{   
    cobra_process_info_t process_info = node.info;
    switch(process_info.process)
    {
        case process_mode_button_press_long:
            press_mode_button_long(cobra_state);
            break;
        case process_mode_button_press_short:
            press_mode_button_short(cobra_state);
            break;
        case process_comms_button_press_long:
            press_comms_button_long(cobra_state);
            break;
        case process_comms_button_press_short:
            press_comms_button_short(cobra_state);
            break;
        case process_alarm_went_off:
            break;
        case process_message_received:
            break;
        default:
            break;
    }

}

void pop_process(void *args)
/* pop node off the queue for processing*/
{
    while(1) {
        cobra_state_struct_t *cobra_state = (cobra_state_struct_t*) args;
        cobra_process_node_t *node = cobra_queue.first;
        if (node!=NULL) {
            cobra_queue.first = node->next;
            process_node(*node, cobra_state);
            free(node);
            cobra_queue.size--;
            print_queue();
        }
        vTaskDelay(50/1000);
    }
    
}


#endif