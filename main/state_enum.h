#pragma once

#include "cobra_colors.h"
#include "cobra_roles.h"

typedef enum {
  mode_music,
  mode_timer,
  mode_locator,
  mode_comms,
  num_of_modes
}cobra_mode_t;

typedef enum {
  state_startup,
  state_sync,
  state_music,
  state_timer,
  state_locator,
  state_group_owner_active,
  state_group_owner_passive,
  state_listener_passive,
  state_listener_active,
  num_of_states
}cobra_state_t;

typedef struct cobra_addr_t{
  uint16_t min_addr;
  uint16_t max_addr;

}cobra_addr_t;

typedef struct {
  cobra_mode_t current_mode;
  cobra_state_t current_state;
  cobra_state_t next_state;
  cobra_role_t group_role;
  cobra_colors_t user_color;
  cobra_addr_t usr_addr;
}cobra_state_struct_t;
