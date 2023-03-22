#pragma once
typedef enum {
  state_startup,
  state_music,
  state_timer,
  state_sync,
  state_group_owner_active,
  state_group_owner_passive,
  state_listener_passive,
  state_listener_active
}cobra_state_t;

typedef struct {
  cobra_state_t current_state;
  cobra_state_t next_state;
}cobra_state_struct_t;
