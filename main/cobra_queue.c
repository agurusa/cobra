#ifndef _COBRA_QUEUE_GUARD
#define _COBRA_QUEUE_GUARD

#include <string.h>

#include "cobra_process.h"
#include "cobra_errors.h"
#include "mode_button.c"
#include "comms_button.c"
#include "ble_responses.c"
#include "static_members.c"

const char * QUEUE_TAG = "QUEUE";

/* return the node where this should be placed AFTER in the queue. IE, how quickly it should be processed. 
*/
cobra_process_node_t * get_node(cobra_priority_t priority) {
    cobra_process_node_t * node =  cobra_queue.first;
    if (node == NULL)
    {
        return node;
    }
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
    ESP_LOGI(QUEUE_TAG, "queue is now:");
    while(node!=NULL)
    {
        if (node->info.process >= num_of_processes || node->info.priority >=num_of_priorities) {
            ESP_LOGE(QUEUE_TAG, "%i, %i", node->info.process, node->info.priority);
        }
        else {
            ESP_LOGI(QUEUE_TAG, "%i, %i", node->info.process, node->info.priority);
        }
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
    cobra_process_node_t * node = get_node(info.priority);
    if (node == NULL) /*queue is currently empty*/
    {
        cobra_queue.first = new_node;
    }
    else
    {
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
            /*pop a message off the BT queue for processing*/
            process_msg();
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