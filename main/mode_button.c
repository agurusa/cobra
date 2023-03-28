#ifndef _MODE_BUTTON_GUARD
#define _MODE_BUTTON_GUARD

#include "state_enum.h"

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

#endif 