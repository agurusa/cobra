#ifndef _BLE_RESPONSES_GUARD
#define _BLE_RESPONSES_GUARD

#include "cobra_process.h"
#include "static_members.c"

//TODO: make this a class

const char* BLE_QUEUE = "MSG_QUEUE";
static cobra_message_queue_t rxn_messages = {
    .first = NULL,
};

bool empty()
{
    return rxn_messages.first == NULL;
};

cobra_bt_response_t* pop_msg()
{
    cobra_bt_response_t* head = rxn_messages.first;
    if (!empty()){
        rxn_messages.first = rxn_messages.first->next;
    }
    return head;
};

void print_msg(cobra_bt_response_t *rsp)
{
    esp_ble_mesh_generic_server_cb_event_t event = (esp_ble_mesh_generic_server_cb_event_t) rsp->event.server;
    ESP_LOGI(BLE_QUEUE, "msg: %i, next: %s, event: %i", rsp->response, rsp->next == NULL? "NULL" : "not null", event);

};

void print_msg_queue(cobra_message_queue_t queue)
{
    cobra_bt_response_t * node = rxn_messages.first;
    while(node!=NULL)
    {
        print_msg(node);
        node = node->next;
    }
};

void push_msg(cobra_bt_response_t *rsp)
{

    // print_msg(rsp); TODO: figure out why this is crashing (dangling pointer?)
    if(empty())
    {
        rxn_messages.first = rsp;
    }
    else
    {
        cobra_bt_response_t * node = rxn_messages.first;
        while(node!=NULL) {
            node = node->next;
        }
        node = rsp;
    }

};

void process_msg()
{
    cobra_bt_response_t *rsp = pop_msg();

    /*changes to comms mode*/
    msg_received = true;
    switch(rsp->response)
    {
        case message_acknowledged:
            break;
        case message_silenced:
            break;
        case message_snoozed:
            break;
        case message_group_owner:
            // everything that needs to happen here is taken care of in the state_updater.
            break;
        case message_location_requested:
            break;
        case message_from_phone_app:
            //change the role of the bracelet to indicate setting sent by phone app
            if (rsp->param->set_val_usr_role != cobra_role){
                cobra_role = rsp->param->set_val_usr_role;
                cobra_role_changed = true;
            }
            break;
        default:
            break;
    }
    free(rsp);
}






#endif