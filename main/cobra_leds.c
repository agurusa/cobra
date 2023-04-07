#ifndef _COBRA_LEDS_GUARD
#define _COBRA_LEDS_GUARD

#include "esp_err.h"
#include "driver/gpio.h"
#include "led_strip.h"
#include "cobra_led_struct.h"
#include "static_members.c"
#include "state_enum.h"

/* For when we start a background task that blinks the LED */
static int led_on = 0;
static TaskHandle_t led_blink_handle = NULL;

const char * LED_TAG = "LED";

led_strip_handle_t led_strip;


const int NUM_LEDS = 30;
const int LED_GPIO = GPIO_NUM_5;
// const int COMMS_LED_INDEX = 1;
const int COMMS_LED_INDEX = 0; /*debug only*/
const int STARTUP_LED_INDEX = 0;

led_struct_t *led_struct = NULL; /*for blinking LEDs*/



led_strip_config_t strip_config = {
    .strip_gpio_num = LED_GPIO,
    .max_leds = NUM_LEDS,
    .led_pixel_format = LED_PIXEL_FORMAT_GRB,
    .led_model = LED_MODEL_WS2812,
};

led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 10 * 1000 * 1000, /*10MHz*/
    .mem_block_symbols = 0, /* use default */
    .flags.with_dma = false,
};

esp_err_t setupBodyLeds()
{
    esp_err_t err = ESP_OK;
    err = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    led_strip_clear(led_strip);
    return err;
}

/**************/
/* debug only */
/**************/
const int BOARD_LED_GPIO = GPIO_NUM_8; /* debug only */
led_strip_handle_t board_led;
led_strip_config_t board_led_config = {
    .strip_gpio_num = BOARD_LED_GPIO,
    .max_leds = 1,
    .led_pixel_format = LED_PIXEL_FORMAT_GRB,
    .led_model = LED_MODEL_WS2812,
};
led_strip_rmt_config_t board_rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 10 * 1000 * 1000, /*10MHz*/
    .mem_block_symbols = 0, /* use default */
    .flags.with_dma = false,
};
esp_err_t setupBoardLed()
{
    esp_err_t err = ESP_OK;
    err = led_strip_new_rmt_device(&board_led_config, &board_rmt_config, &board_led);
    led_strip_clear(board_led);
    return err;
}
/**************/
/**************/

void blink_led(void* args)
{

    led_struct_t* led_args = (led_struct_t*) args;
    while(1) {
        if (led_on) {
            led_strip_set_pixel(led_args->strip, led_args->index, led_args->red, led_args->green, led_args->blue);
        }
        else {
            led_strip_set_pixel(led_args->strip, led_args->index, 0,0,0);
        }
        led_on = !led_on;
        led_strip_refresh(led_args->strip);
        vTaskDelay(10);
    }

}

esp_err_t fillBodyLeds(cobra_state_t state, led_strip_handle_t strip)
// fill body LEDs with the correct color combo
{   
    if(state!=state_listener_active && state!=state_timer)
    {
        // Use the handle to delete the task.
        if( led_blink_handle != NULL )
        {
            vTaskDelete( led_blink_handle );
            led_blink_handle = NULL;
            free(led_struct);
        }
    }

    esp_err_t err = ESP_OK;
    switch (state) {
        case state_music:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, state*10,state*10,state*20);
            break;
        case state_timer:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, state*10,state*10,state*20);
            break;
        case state_sync:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, state*10,state*10,state*20);
            break;
        case state_startup:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, state*10,state*10,state*20);
            break;
        case state_group_owner_active:
            break;
        case state_group_owner_passive:
            break;
        case state_listener_passive:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, 0, 100, 0);
            break;
        case state_listener_active:
            led_struct_t *led_args = calloc(1, sizeof(led_struct_t));
            led_args->strip = strip;
            led_args->index = COMMS_LED_INDEX;
            led_args->red = 100;
            led_args->green = 0;
            led_args->blue = 0;
            led_struct = led_args;
            xTaskCreate(blink_led, "BLINK_TASK", STACK_SIZE, led_struct, tskIDLE_PRIORITY, &led_blink_handle);
            break;
        default:
            break;
    }
    err = led_strip_refresh(strip);
    return err;
}

#endif