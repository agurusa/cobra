#include <stdbool.h>
#include "static_members.h"
#include "cobra_roles.h"
#include "cobra_colors.h"

bool msg_received = false;
cobra_role_t cobra_role = role_listener;
bool cobra_role_changed = false;
cobra_colors_t usr_colors[NUM_LEDS] = {0};
uint16_t usr_addrs[NUM_LEDS] = {0};

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

void update_usr_colors(uint16_t usr_addr, cobra_colors_t color) {
    //TODO: Get the index for the usr addr
    int usr_addr_index = FIRST_USR_LED_INDEX;
    usr_colors[usr_addr_index] = color;
}

cobra_colors_t get_usr_color(int usr_index) {
    return usr_colors[usr_index];
}