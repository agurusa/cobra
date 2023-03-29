#ifndef _COMMS_BUTTON_GUARD
#define _COMMS_BUTTON_GUARD


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
            /*update state to active*/
            
            break;
        case state_listener_active:
            /*acknowledge*/
            /*update state to passive*/
            break;
        default:
            break;
    }
}

#endif