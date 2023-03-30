#ifndef _COMMS_BUTTON_GUARD
#define _COMMS_BUTTON_GUARD

#include "generic_onoff_client_model.c"


void press_comms_button_long(cobra_state_struct_t *cobra_state)
{
    switch(cobra_state->current_state)
    {
        case state_listener_active:
            /*snooze (remind user to respond in 5 min)*/
            /*update state to passive*/
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
            /*update state to active*/

            break;
        case state_listener_active:
            /*generic_onoff_server model set rx event would have put us in this state*/
            /*acknowledge*/
            /*light_hsl_client model set tx*/
            /*update state to passive*/
            break;
        default:
            break;
    }
}

#endif