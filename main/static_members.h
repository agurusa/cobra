#pragma once 

#include <stdbool.h>
#include <inttypes.h>

#include "cobra_roles.h"
#include "cobra_colors.h"
#include "state_enum.h"

#define QUEUE_SIZE 10
#define STACK_SIZE  2048
#define GROUP_ADDR 0xC000 /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
#define COBRA_ROLE_ID 0x1111
#define NUM_LEDS 10
#define FIRST_USR_LED_INDEX 2
#define NUM_LISTENERS_IN_GROUP 1 /*TODO: get this from the configuration client?*/
/*TODO: update these based on what is gathered by the app sync*/
#define USER_LIGHTNESS 20
#define USER_HUE 20
#define USER_SATURATION 20

extern cobra_state_struct_t cobra_state;
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
extern void update_usr_addrs(uint16_t usr_addr, int index);
extern uint16_t get_usr_addr(int index);
extern cobra_state_struct_t get_cobra_state();
extern void set_cobra_state_struct(cobra_state_struct_t state);
extern void set_current_mode(cobra_mode_t mode);
extern void set_next_state(cobra_state_t next_state);
extern void set_current_state(
    cobra_state_t current_state
);