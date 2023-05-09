#ifndef _STATIC_MEMBERS_GUARD
#define _STATIC_MEMBERS_GUARD

#include <stdbool.h>

#define QUEUE_SIZE 10
#define STACK_SIZE  2048
#define GROUP_ADDR 0xC000 /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
#define COBRA_ROLE_ID 0x1111

static bool msg_received = false;

#endif