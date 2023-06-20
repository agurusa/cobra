#ifndef _MODE_BUTTON_GUARD
#define _MODE_BUTTON_GUARD

#include "state_enum.h"
#include "light_hsl_client_model.c"
#include "cobra_colors.h"

const char * MODE_TAG = "MODE";

void update_mode(cobra_mode_t *current_mode)
{
    if (*current_mode < num_of_modes-1) {
        *current_mode = *current_mode + 1;
    }
    else{
        *current_mode = 0;
    }
}

void press_mode_button_short(cobra_state_struct_t *cobra_state)
{
    update_mode(&cobra_state->current_mode);
}

void press_mode_button_long(cobra_state_struct_t *cobra_state)
{
    switch(cobra_state->current_state)
    {
        case state_listener_active:
            /*silence*/
            send_hsl_set_message(RED);
            /*update state to passive*/
            update_msg_received(false);
            break;
        default:
            break;
    }
}

#endif 