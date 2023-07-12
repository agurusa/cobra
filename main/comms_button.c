#ifndef _COMMS_BUTTON_GUARD
#define _COMMS_BUTTON_GUARD

#include "generic_onoff_client_model.c"
#include "light_hsl_client_model.c"
#include "static_members.h"
#include "cobra_colors.h"

const char * COMMS_TAG = "COMMS";


void press_comms_button_long()
{
    cobra_state_struct_t cobra_state = get_cobra_state();
    switch(cobra_state.current_state)
    {
        case state_listener_active:
            /*snooze (remind user to respond in 5 min)*/
            /*update state to passive*/
            send_hsl_set_message(YELLOW);
            update_msg_received(false);
            /*TODO:start timer countown to process this again*/
            break;
        default:
            break;
    }
}

void press_comms_button_short()
{
    cobra_state_struct_t cobra_state = get_cobra_state();
    switch(cobra_state.current_state)
    {
        case state_group_owner_passive:
            /*send out message to all group listeners*/
            /*generic_onoff_client model set tx*/
            send_gen_onoff_set();
            cobra_colors_t off = {0,0,0};
            update_all_listener_colors(off);
            update_all_listener_msgs_received(false);
            break;
        case state_listener_active:
            /*generic_onoff_server model set rx event would have put us in this state*/
            /*acknowledge*/
            ESP_LOGE(COMMS_TAG, "acknowleding");
            /*light_hsl_client model set tx*/
            send_hsl_set_message(cobra_state.user_color);
            /*update state to passive*/
            update_msg_received(false);
            set_responded(true);
            break;
        default:
            break;
    }
}

#endif