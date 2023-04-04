#ifndef _BLE_RESPONSES_GUARD
#define _BLE_RESPONSES_GUARD

#include "cobra_process.h"

//TODO: make this a class

const char* BLE_QUEUE = "MSG_QUEUE";

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

void print_msg(cobra_bt_response_t *rsp)
{
    esp_ble_mesh_generic_server_cb_param_t * param = rsp->param->server;
    esp_ble_mesh_generic_server_cb_event_t event = (esp_ble_mesh_generic_server_cb_event_t) rsp->event.server;
    ESP_LOGE(BLE_QUEUE, "msg: %i, next: %s, event: %i, param net idx: 0x%04x", rsp->response, rsp->next == NULL? "NULL" : "not null", event, param->ctx.net_idx);

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

    print_msg(rsp);
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
            ESP_LOGE("BLE_RESPONSES", "message group owner rcv'd!");
            break;
        case message_location_requested:
            break;
        default:
            break;
    }
    free(rsp);
}






#endif