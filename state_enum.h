enum class State{
  music,
  timer,
  sync,
};

State AllStates[3] = {State::music, State::timer, State::sync};

typedef struct {
  int state_index;
  State state;
}state_struct_t;
