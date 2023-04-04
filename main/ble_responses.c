#ifndef _BLE_RESPONSES_GUARD
#define _BLE_RESPONSES_GUARD

#include "cobra_process.h"

//TODO: make this a class

static cobra_message_queue_t rxn_messages = {
    .first = NULL,
};
static bool group_owner_msg_received = false;


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

void push_msg(cobra_bt_response_t *rsp)
{
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
    switch(rsp->response)
    {
        case message_acknowledged:
            break;
        case message_silenced:
            break;
        case message_snoozed:
            break;
        case message_group_owner:
            /*changes to listener active state*/
            group_owner_msg_received = true;
            break;
        case message_location_requested:
            break;
        default:
            break;
    }
    free(rsp);
}






#endif