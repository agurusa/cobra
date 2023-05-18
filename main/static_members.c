#include <stdbool.h>
#include "static_members.h"
#include "cobra_roles.h"

bool msg_received = false;
cobra_role_t cobra_role = role_listener;
bool cobra_role_changed = false;

void update_msg_received(bool val){
    msg_received = val;
}