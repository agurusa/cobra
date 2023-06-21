#include <stdbool.h>
#include "static_members.h"
#include "cobra_roles.h"
#include "cobra_colors.h"

bool msg_received = false;
cobra_role_t cobra_role = role_listener;
bool cobra_role_changed = false;
cobra_colors_t usr_colors[NUM_LEDS] = {0};
uint16_t usr_addrs[NUM_LEDS] = {0};
bool usr_msgs_received[NUM_LEDS] = {true, true, true, true, true, true, true, true, true, true};

void update_msg_received(bool val){
    msg_received = val;
}

//TODO
const cobra_colors_t RED = {
    50,
    50,
    50
};

//TODO
const cobra_colors_t YELLOW = {
    80,
    80,
    80
};

int get_index_for_usr_addr(uint16_t usr_addr)
{
    //TODO: Get the index for the usr addr
    return FIRST_USR_LED_INDEX;
}

void update_usr_colors(uint16_t usr_addr, cobra_colors_t color) {
    int usr_addr_index = get_index_for_usr_addr(usr_addr);
    usr_colors[usr_addr_index] = color;
}

cobra_colors_t get_usr_color(int usr_index) {
    return usr_colors[usr_index];
}

void update_all_usr_colors(cobra_colors_t color) {
    for(int i = FIRST_USR_LED_INDEX; i < FIRST_USR_LED_INDEX+NUM_LISTENERS_IN_GROUP; i++){
        usr_colors[i] = color;
    }
}

void update_usr_msgs_received(uint16_t usr_addr, bool rcvd){
    int usr_addr_index = get_index_for_usr_addr(usr_addr);
    usr_msgs_received[usr_addr_index] = rcvd;
}

void update_all_usr_msgs_received(bool rcvd){
    for(int i = FIRST_USR_LED_INDEX; i < FIRST_USR_LED_INDEX+NUM_LISTENERS_IN_GROUP; i++){
        usr_msgs_received[i] = rcvd;
    }
}

bool all_msgs_received(){
    for(int i = FIRST_USR_LED_INDEX; i < FIRST_USR_LED_INDEX+NUM_LISTENERS_IN_GROUP; i++){
        if(!usr_msgs_received[i]){
            return false;
        }
    }
    return true;
}