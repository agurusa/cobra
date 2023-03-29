#ifndef _STATE_UPDATER_GUARD
#define _STATE_UPDATER_GUARD

#include "state_enum.h"
#include "cobra_leds.c"
#include "cobra_roles.h"
#include "periodic_timer.c"

/* Updates the state machine */

const char * STATE_TAG = "STATE";

void update_state(cobra_state_struct_t *cobra_state)
{
    cobra_state_t current_state = (*cobra_state).current_state;
    cobra_state_t next_state;
    switch(cobra_state->current_mode)
    {
        case mode_music:
            next_state = state_music;
            break;
        case mode_timer:
            next_state = state_timer;
            break;
        case mode_comms:
            if (cobra_state->group_role == role_owner)
            {
                /*if we're not waiting on anyone to respond, we should be in the passive state*/
                next_state = state_group_owner_passive;
                /*if we're waiting on folks to respond to a message, we should be in the active state*/
            }
            else
            {
                /*if we don't need to respond to any messages, we should be in the passive state*/
                next_state = state_listener_passive;
                /*if there is a message to respond to, then we should be in the active state*/
            }
            break;
        case mode_locator:
            next_state = state_locator;
            break;
        default:
            next_state = current_state;
            break;
    }
    (*cobra_state).next_state = next_state;
}

// see if there is a need for a state change.
// if there is, then change the .next_state for background task processing.
void state_isr_callback(void * args) {
    cobra_state_struct_t *cobra_state = (cobra_state_struct_t*) args;
    update_state(cobra_state);
}

void respond_to_state_change(void * args)
{   
    while(1) {
        cobra_state_struct_t * state =  (cobra_state_struct_t *) args;
        cobra_state_t current_state = (*state).current_state;
        cobra_state_t next_state = (*state).next_state;
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
            fillBodyLeds(next_state, board_led);
            // update the current state to reflect the new state
            (*state).current_state = next_state; 
            ESP_LOGE(STATE_TAG, "current state now: %i", (*state).current_state);

        }
        vTaskDelay(50/1000);
    }
}

#endif