#ifndef _BLE_RESPONSES_GUARD
#define _BLE_RESPONSES_GUARD

#include "cobra_process.h"
#include "cobra_leds.c"
#include "static_members.h"

//TODO: make this a class

const char* BLE_QUEUE = "MSG_QUEUE";


void print_msg(cobra_bt_response_t *rsp)
{
    esp_ble_mesh_generic_server_cb_event_t event = (esp_ble_mesh_generic_server_cb_event_t) rsp->event.server;
    ESP_LOGI(BLE_QUEUE, "msg: %i, event: %i", rsp->response, event);

};


void process_msg()
{
    cobra_bt_response_t *rsp;
    const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
    if(xQueueReceive(bleMessageQueue, &rsp, xBlockTime)){
        /*changes to comms mode*/
        update_msg_received(true); 
        switch(rsp->response)
        {
            case message_acknowledged:
                ESP_LOGI(BLE_QUEUE, "message acknoweldged received!");
                uint16_t recv_addr = rsp->param->set_val_comms_color.recv_addr;
                update_usr_msgs_received(recv_addr, true);
                ESP_LOGE(BLE_QUEUE, "rxn color is: lightness %u, hue %u, saturation %u from 0x%04x", rsp->param->set_val_comms_color.lightness, rsp->param->set_val_comms_color.hue, rsp->param->set_val_comms_color.saturation, 
                rsp->param->set_val_comms_color.recv_addr);
                /*update the LED index of the associated user to the color received*/
                update_usr_color(rsp->param->set_val_comms_color);
                /*refresh the led strip.*/
                set_usr_led_changed(true);
                update_msg_received(false); 
                break;
            case message_silenced:
                break;
            case message_snoozed:
                break;
            case message_group_owner:
                ESP_LOGE(BLE_QUEUE, "msg received from group owner"); 
                cobra_colors_t off = {0,0,0};
                set_responded(false);
                update_all_usr_colors(off);
                break;
            case message_location_requested:
                break;
            case message_role_changed:
                //change the role of the bracelet to indicate setting sent by phone app
                cobra_role_t current_role = get_cobra_role();
                if (rsp->param->set_val_usr_role != current_role){
                    set_cobra_group_role(rsp->param->set_val_usr_role);

                }
                update_msg_received(false);
                break;
            case message_usr_addr:
                uint16_t addr = rsp->param->set_val_cobra_usr.recv_addr;
                int index = rsp->param->set_val_cobra_usr.recv_index;
                int usr_role = rsp->param->set_val_cobra_usr.group_role;
                if (get_usr_addr_by_index(index)!= addr){
                    update_usr_addrs(addr, index);
                }
                /*if receiving info about this bracelet, update the cobra state info*/
                cobra_addr_t usr_addr = get_usr_addr();
                if(addr >= usr_addr.min_addr && addr <= usr_addr.max_addr){
                    set_cobra_group_role(usr_role);
                }
                update_msg_received(false);
                break;
            default:
                break;
        }
        vPortFree(rsp->param);
        vPortFree(rsp);
    }
    else{
        ESP_LOGE(BLE_QUEUE, "NO BLE MSG IN QUEUE?? xQueueReceive returned false!");
    }
}






#endif