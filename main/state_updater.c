#ifndef _STATE_UPDATER_GUARD
#define _STATE_UPDATER_GUARD

#include "state_enum.h"
#include "cobra_leds.c"
#include "cobra_roles.h"
#include "periodic_timer.c"

/* Updates the state machine */

const char * STATE_TAG = "STATE";

cobra_state_struct_t cobra_state = {
    .current_state = state_startup,
    .next_state = state_startup,
    .group_role = role_owner,
};


void update_state(cobra_state_struct_t *cobra_state)
{
    cobra_state_t current_state = cobra_state->current_state;
    cobra_state_t next_state;
    if (current_state == state_startup)
    {
        next_state = state_music;
    }
    else if (current_state == state_music)
    {
        next_state = state_timer;
    }
    else if (current_state == state_timer)
    {
        next_state = state_music;
    }
    else 
    {
        next_state = state_timer;
    }
    cobra_state->next_state = next_state;
}

// see if there is a need for a state change.
// if there is, then change the .next_state for background task processing.
void state_isr_callback(void * args) {
    update_state(&cobra_state);
}

void respond_to_state_change(void * args)
{   
    while(1) {
        cobra_state_struct_t * state =  (cobra_state_struct_t *) args;
        cobra_state_t current_state = state->current_state;
        cobra_state_t next_state = state->next_state;
        if (current_state != next_state)
        {
            ESP_LOGE(STATE_TAG, "update state from %i to %i", current_state, next_state);
            // device responds to state change
            switch (next_state){
                case state_startup: 
                    fillBodyLeds(next_state, board_led);
                    break;
                case state_timer:
                    fillBodyLeds(next_state, board_led);
                    break;
                case state_music:
                    fillBodyLeds(next_state, board_led);
                    break;
                default:
                    break;
            }
            // update the current state to reflect the new state
            state->current_state = next_state;
        }
        vTaskDelay(50/1000);
    }
}

#endif