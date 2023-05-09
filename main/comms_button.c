#ifndef _COMMS_BUTTON_GUARD
#define _COMMS_BUTTON_GUARD

#include "generic_onoff_client_model.c"
#include "light_hsl_client_model.c"
#include "static_members.c"
#include "cobra_colors.h"

const char * COMMS_TAG = "COMMS";


void press_comms_button_long(cobra_state_struct_t *cobra_state)
{
    switch(cobra_state->current_state)
    {
        case state_listener_active:
            /*snooze (remind user to respond in 5 min)*/
            /*update state to passive*/
            send_hsl_set_message(YELLOW);
            msg_received = false;
            /*TODO:start timer countown to process this again*/
            break;
        default:
            break;
    }
}

void press_comms_button_short(cobra_state_struct_t *cobra_state)
{
    switch(cobra_state->current_state)
    {
        case state_group_owner_passive:
            /*send out message to all group listeners*/
            /*generic_onoff_client model set tx*/
            send_gen_onoff_set();
            /*state updated to active on ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT*/
            break;
        case state_listener_active:
            /*generic_onoff_server model set rx event would have put us in this state*/
            /*acknowledge*/
            ESP_LOGE(COMMS_TAG, "acknowleding");
            /*light_hsl_client model set tx*/
            send_hsl_set_message(cobra_state->user_color);
            /*update state to passive*/
            msg_received = false;
            break;
        default:
            break;
    }
}

#endif