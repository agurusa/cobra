#pragma once 

#include <stdbool.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "cobra_roles.h"
#include "cobra_colors.h"
#include "state_enum.h"

#define QUEUE_SIZE 10
#define STACK_SIZE  2048
#define GROUP_ADDR 0xC000 /* TODO: get this from the configuration client. Group Address assigned to all Group Members */
#define MAX_USR_ADDR 0X7FFF
#define COBRA_ROLE_ID 0x1111
#define NUM_LEDS 10
#define FIRST_USR_LED_INDEX 2
#define USER_LIGHTNESS 50
#define USER_HUE 252
#define USER_SATURATION 100

/*incoming BLE messages to be processed*/
extern QueueHandle_t bleMessageQueue;
extern cobra_state_struct_t cobra_state;
extern bool msg_received;
extern bool get_msg_received();
extern void update_msg_received(bool val);
extern const cobra_colors_t RED;
extern const cobra_colors_t YELLOW;
extern bool usr_msgs_received[NUM_LEDS];
extern uint16_t usr_addrs[NUM_LEDS];
extern cobra_colors_t usr_colors[NUM_LEDS];
extern void update_usr_colors(cobra_colors_t color);
extern cobra_colors_t get_usr_color(int usr_index);
extern void update_all_usr_colors(cobra_colors_t color);
extern void update_usr_msgs_received(uint16_t usr_addr, bool rcvd);
extern void update_all_usr_msgs_received(bool rcvd);
extern bool all_msgs_received();
extern void update_usr_addrs(uint16_t usr_addr, int index);
extern uint16_t get_usr_addr_by_index(int index);
extern cobra_state_struct_t get_cobra_state();
extern void set_cobra_state_struct(cobra_state_struct_t state);
extern void set_current_mode(cobra_mode_t mode);
extern void set_next_state(cobra_state_t next_state);
extern void set_current_state(cobra_state_t current_state);
extern void set_cobra_group_role(cobra_role_t role);
extern cobra_role_t get_cobra_role();
extern int get_index_for_usr_addr(uint16_t usr_addr);
int get_num_listeners_in_group();
extern void set_usr_addr(uint16_t usr_addr);
extern cobra_addr_t get_usr_addr();
extern bool get_responded();
extern void set_responded(bool responded);

