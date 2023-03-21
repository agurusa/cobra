#pragma once
typedef enum {
  state_music,
  state_timer,
  state_sync,
}cobra_state_t;

cobra_state_t AllStates[3] = {state_music, state_timer, state_sync};

typedef struct {
  int state_index;
  cobra_state_t state;
}cobra_state_struct;
