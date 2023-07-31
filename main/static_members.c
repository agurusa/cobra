#include <stdbool.h>

#include "esp_log.h"

#include "static_members.h"
#include "cobra_roles.h"
#include "cobra_colors.h"
#include "state_enum.h"

QueueHandle_t bleMessageQueue = NULL;
bool msg_received = false;
cobra_colors_t usr_colors[NUM_LEDS] = {0}; //TODO: cobra_colors_t also contains recv_addr. we could use that to update the correct index.
uint16_t usr_addrs[NUM_LEDS] = {0};
bool usr_msgs_received[NUM_LEDS] = {true, true, true, true, true, true, true, true, true, true};
const char * STATIC_MEM_TAG = "EXTERN";
int group_owner_index = 0;

cobra_colors_t user_color = {
    .lightness = USER_LIGHTNESS,
    .hue = USER_HUE,
    .saturation = USER_SATURATION,
};
const cobra_addr_t startup_addr = {
    .min_addr = MAX_USR_ADDR,
    .max_addr = 0
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
    .user_color = startup_color,
    .usr_addr = startup_addr,
    .responded = true,
    .usr_led_changed = false
};

bool get_msg_received(){
    return msg_received;
}

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
    //return the index associated with the received address. Note that is a range, because the primary unicast address is only the address of the Node. Each node has a series of elements, which have their own addresses.
    int last_index = get_num_listeners_in_group();
    for(int i = 0; i < last_index; i++){
        if(i == last_index - 1){
            if (usr_addrs[i]<= usr_addr && MAX_USR_ADDR > usr_addr){
                return i;
            }
        }
        else{
            if (usr_addrs[i] <= usr_addr && usr_addrs[i+1] > usr_addr){
                return i;
            }
        }
    }
    ESP_LOGE(STATIC_MEM_TAG, "Unknown usr address: 0x%04x", usr_addr);
    return NUM_LEDS; //consuming func responsible for understanding that this means the usr wasn't found.
}

void update_usr_colors(cobra_colors_t color) {
    int usr_addr_index = get_index_for_usr_addr(color.recv_addr);
    if (usr_addr_index == NUM_LEDS){ //usr not found
        return;
    }
    usr_addr_index+= FIRST_USR_LED_INDEX;
    usr_colors[usr_addr_index] = color;
}

cobra_colors_t get_usr_color(int usr_index) {
    return usr_colors[usr_index];
}

void update_all_listener_colors(cobra_colors_t color) {
    for(int i = FIRST_USR_LED_INDEX; i < FIRST_USR_LED_INDEX+get_num_listeners_in_group(); i++){
        if(i!= group_owner_index+FIRST_USR_LED_INDEX){
            usr_colors[i] = color;
            
        }
    }
}

void update_usr_msgs_received(uint16_t usr_addr, bool rcvd){
    int usr_addr_index = get_index_for_usr_addr(usr_addr);
    if (usr_addr_index == NUM_LEDS){ //usr not found
        return;
    }
    usr_addr_index+=FIRST_USR_LED_INDEX;
    usr_msgs_received[usr_addr_index] = rcvd;
}

void update_all_listener_msgs_received(bool rcvd){
    for(int i = FIRST_USR_LED_INDEX; i < FIRST_USR_LED_INDEX+get_num_listeners_in_group(); i++){
        if(i!= group_owner_index+FIRST_USR_LED_INDEX){
            usr_msgs_received[i] = rcvd;
        }
    }
}

bool all_msgs_received(){
    for(int i = FIRST_USR_LED_INDEX; i < FIRST_USR_LED_INDEX+get_num_listeners_in_group(); i++){
        if(!usr_msgs_received[i]){
            return false;
        }
    }
    return true;
}

void update_usr_addrs(uint16_t usr_addr, int index){
    usr_addrs[index] = usr_addr;
    ESP_LOGI(STATIC_MEM_TAG, "Updating usr addr at %i to 0x%04x", index, usr_addrs[index]);
}

uint16_t get_usr_addr_by_index(int index){
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

extern void set_current_state(cobra_state_t current_state)
{
    cobra_state.current_state = current_state;
}

void set_cobra_group_role(cobra_role_t role)
{
    ESP_LOGI(STATIC_MEM_TAG, "NOW CHANGING ROLE TO: %u", role);
    cobra_state.group_role = role;
}

extern cobra_role_t get_cobra_role()
{
    return cobra_state.group_role;
}

int get_num_listeners_in_group(){
    int num = 0;
    for(int i = 0; i < NUM_LEDS; i++){
        /*TODO: clear all addresses so we can deal with users that have been removed from the group*/
        if(!usr_addrs[i]){
            return num;
        }
        num++;
    }
    return num;
}

void set_usr_addr(uint16_t usr_addr){
    if(cobra_state.usr_addr.min_addr > usr_addr){
        cobra_state.usr_addr.min_addr = usr_addr;
    }
    if(cobra_state.usr_addr.max_addr < usr_addr){
        cobra_state.usr_addr.max_addr = usr_addr;
    }
}

cobra_addr_t get_usr_addr(){
    return cobra_state.usr_addr;
}

bool get_responded(){
    return cobra_state.responded;
}

void set_responded(bool responded){
    cobra_state.responded = responded;
}

void set_usr_led_changed(bool changed){
    cobra_state.usr_led_changed = changed;
}

bool get_usr_led_changed(){
    return cobra_state.usr_led_changed;
}

void set_group_owner_index(int index){
    group_owner_index = index;
}
int get_group_owner_index(){
    return group_owner_index;
}