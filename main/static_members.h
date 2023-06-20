#pragma once 

#include <stdbool.h>
#include "cobra_roles.h"

#define QUEUE_SIZE 10
#define STACK_SIZE  2048
#define GROUP_ADDR 0xC000 /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
#define COBRA_ROLE_ID 0x1111

extern bool msg_received;
extern cobra_role_t cobra_role;
extern bool cobra_role_changed;
extern void update_msg_received(bool val);
