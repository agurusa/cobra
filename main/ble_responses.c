#ifndef _BLE_RESPONSES_GUARD
#define _BLE_RESPONSES_GUARD

#include "cobra_process.h"
#include "cobra_leds.c"
#include "static_members.h"

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
    update_msg_received(true); /*in order for this to work on the listening group's bracelets, the hsl server node cannot be subscribed to the festival group (otherwise this will log as a received message)*/
    switch(rsp->response)
    {
        case message_acknowledged:
            ESP_LOGI(BLE_QUEUE, "message acknoweldged received!");
            update_usr_msgs_received(rsp->param->recv_addr, true);
            /*get color from rsp param*/
            //TODO: some kind of memory corruption is potentially cuasing the lightness value here to be inconsistent. (280 when it should be 20)
            ESP_LOGE(BLE_QUEUE, "rxn color is: lightness %u, hue %u, saturation %u", rsp->param->set_val_comms_color.lightness, rsp->param->set_val_comms_color.hue, rsp->param->set_val_comms_color.saturation);
            /*update the LED index of the associated user to the color received*/
            update_usr_color(rsp->param->recv_addr, rsp->param->set_val_comms_color);
            update_msg_received(false);
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
            update_msg_received(false);
            break;
        default:
            break;
    }
    free(rsp->param);
    free(rsp);
}






#endif