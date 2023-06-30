#include <stdbool.h>

#include "esp_log.h"

#include "static_members.h"
#include "cobra_roles.h"
#include "cobra_colors.h"
#include "state_enum.h"

bool msg_received = false;
cobra_role_t cobra_role = role_listener;
bool cobra_role_changed = false;
cobra_colors_t usr_colors[NUM_LEDS] = {0};
uint16_t usr_addrs[NUM_LEDS] = {0};
bool usr_msgs_received[NUM_LEDS] = {true, true, true, true, true, true, true, true, true, true};

cobra_colors_t user_color = {
    .lightness = USER_LIGHTNESS,
    .hue = USER_HUE,
    .saturation = USER_SATURATION,
};

const cobra_role_t startup_role = role_listener;
const cobra_colors_t startup_color = {
    .lightness = USER_LIGHTNESS,
    .hue = USER_HUE,
    .saturation = USER_SATURATION,
};

cobra_state_struct_t cobra_state  = {
    .current_state = state_startup,
    .next_state = state_startup,
    .group_role = startup_role,
    .current_mode = mode_music,
    .user_color = startup_color
};

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

void update_usr_addrs(uint16_t usr_addr, int index){
    usr_addrs[index] = usr_addr;
    ESP_LOGE("debug", "UPDATING USR ADDR at %i to %u", index, usr_addrs[index]);
}

uint16_t get_usr_addr(int index){
    return usr_addrs[index];
}

extern cobra_state_struct_t get_cobra_state(){
    return cobra_state;
}
extern void set_cobra_state_struct(cobra_state_struct_t state){
    cobra_state.current_state = state.current_state;
    cobra_state.next_state = state.next_state;
    cobra_state.group_role = state.group_role;
    cobra_state.current_mode = state.current_mode;
    cobra_state.user_color = state.user_color;
}

extern void set_current_mode(cobra_mode_t mode){
    cobra_state.current_mode = mode;
}

extern void set_next_state(cobra_state_t next_state) {
    cobra_state.next_state = next_state;
}

extern void set_current_state(
    cobra_state_t current_state
) {
    cobra_state.current_state = current_state;
}