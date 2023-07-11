#ifndef _STATE_UPDATER_GUARD
#define _STATE_UPDATER_GUARD

#include "state_enum.h"
#include "cobra_leds.c"
#include "cobra_roles.h"
#include "periodic_timer.c"

/* Updates the state machine */

const char * STATE_TAG = "STATE";

void update_state()
{
    cobra_state_struct_t cobra_state = get_cobra_state();
    cobra_state_t current_state = cobra_state.current_state;
    cobra_mode_t current_mode = cobra_state.current_mode;
    cobra_state_t next_state = current_state;
    bool _msg_rcvd = get_msg_received();
    bool responded = get_responded();
    cobra_mode_t next_mode = (_msg_rcvd || !responded)? mode_comms : current_mode; 
    set_current_mode(next_mode);
    switch(cobra_state.current_mode)
    {
        case mode_music:
            next_state = state_music;
            break;
        case mode_timer:
            next_state = state_timer;
            break;
        case mode_comms:
            if (cobra_state.group_role == role_owner)
            {
                /*if we're not waiting on anyone to respond, we should be in the passive state*/
                /*if we're waiting on folks to respond to a message, we should be in the active state*/
                next_state = (all_msgs_received()) ? state_group_owner_passive : state_group_owner_active;
            }
            else
            {
                /*if there is an incoming message OR we haven't responded, we should be in the active state. Otherwise, we're in the passive state.*/
                next_state = (_msg_rcvd || !responded) ? state_listener_active : state_listener_passive;
            }
            break;
        case mode_locator:
            next_state = state_locator;
            break;
        default:
            next_state = current_state;
            break;
    }
    set_next_state(next_state);
}

// see if there is a need for a state change.
// if there is, then change the .next_state for background task processing.
void state_isr_callback(void * args) {
    update_state();
}

void respond_to_state_change(void * args)
{   
    while(1) {
        cobra_state_struct_t state = get_cobra_state();
        cobra_state_t current_state = state.current_state;
        cobra_state_t next_state = state.next_state;
        /*todo: debug why this is happening*/
        if (next_state >= num_of_states) {
            next_state = current_state;
        }
        if (current_state != next_state)
        {
            // device responds to state change
            switch (next_state){
                case state_startup: 
                    break;
                case state_timer:
                    break;
                case state_music:
                    break;
                case state_sync:
                    break;
                case state_locator:
                    break;
                case state_group_owner_active:
                    break;
                case state_group_owner_passive:
                    break;
                case state_listener_passive:
                    break;
                case state_listener_active:
                    break;
                default:
                    break;
            }
            fillBodyLeds(next_state, led_strip);
            // update the current state to reflect the new state
            set_current_state(next_state);
            ESP_LOGE(STATE_TAG, "current state now: %i", get_cobra_state().current_state);

        }
        else if (state.usr_led_changed){
            fillBodyLeds(next_state, led_strip);
            set_usr_led_changed(false);
        }
        vTaskDelay(1);
    }
}

#endif