#pragma once 

#include <stdbool.h>
#include <inttypes.h>

#include "cobra_roles.h"
#include "cobra_colors.h"

#define QUEUE_SIZE 10
#define STACK_SIZE  2048
#define GROUP_ADDR 0xC000 /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
#define COBRA_ROLE_ID 0x1111
#define NUM_LEDS 10
#define FIRST_USR_LED_INDEX 2
#define NUM_LISTENERS_IN_GROUP 1 /*TODO: get this from the configuration client?*/

extern bool msg_received;
extern cobra_role_t cobra_role;
extern bool cobra_role_changed;
extern void update_msg_received(bool val);

extern const cobra_colors_t RED;
extern const cobra_colors_t YELLOW;
extern bool usr_msgs_received[NUM_LEDS];
extern uint16_t usr_addrs[NUM_LEDS];
extern cobra_colors_t usr_colors[NUM_LEDS];
extern void update_usr_colors(uint16_t usr_addr, cobra_colors_t color);
extern cobra_colors_t get_usr_color(int usr_index);
extern void update_all_usr_colors(cobra_colors_t color);
extern void update_usr_msgs_received(uint16_t usr_addr, bool rcvd);
extern void update_all_usr_msgs_received(bool rcvd);
extern bool all_msgs_received();
