#ifndef _COBRA_LEDS_GUARD
#define _COBRA_LEDS_GUARD

#include <stdlib.h>
#include <math.h>

#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_strip.h"
#include "cobra_led_struct.h"
#include "static_members.h"
#include "state_enum.h"

/* For when we start a background task that blinks the LED */
static int led_on = 0;
static TaskHandle_t led_blink_handle = NULL;

const char * LED_TAG = "LED";

led_strip_handle_t led_strip;

const int LED_GPIO = GPIO_NUM_5;
const int COMMS_LED_INDEX = 1;
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

void hsl_to_rgb(uint16_t hue, uint16_t lightness, uint16_t saturation, uint16_t* red, uint16_t* green, uint16_t* blue)
{
    uint16_t h = hue/360;
    uint16_t s = saturation/100;
    uint16_t l = lightness/100;
    uint16_t chroma = (1 - abs(2*l - 1)) * s;
    uint16_t X = chroma * (1 - abs((h / 60) % 2 - 1));
    uint16_t m = l - chroma/2;
    uint16_t r=0;
    uint16_t g=0;
    uint16_t b=0;
    if (0 <= h && h < 60){
        r = chroma;
        g = X;
        b= 0;
    }
    else if (120 <= h && h <180){
        r = 0; 
        g = chroma;
        b= X;
    }
    else if (180 <=h && h <240) {
        r = 0;
        g = X;
        b= chroma;
    }
    else if (240 <= h && h < 300){
        r = X; 
        g = 0;
        b= chroma;
    }
    else if (300 <= h && h <360){
        r = chroma;
        g = 0;
        b= X;
    }
    else {
        ESP_LOGE(LED_TAG, "unknown h value: %u", h);
    }
    *red = round((r + m) * 255);
    *green = round((g + m) * 255);
    *blue = round((b + m) * 255);
}

void update_usr_color(uint16_t usr_addr, cobra_colors_t color)
{
    //TODO: update the specific index associated with the usr_addr. for now, just updating a constant index.
    update_usr_colors(usr_addr, color);

}

void led_strip_set_usr_colors(led_strip_handle_t strip) {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    for (int i = FIRST_USR_LED_INDEX; i< NUM_LEDS; i++){
        cobra_colors_t color = get_usr_color(i);
        hsl_to_rgb(color.hue, color.lightness, color.saturation, &red, &green, &blue);
        led_strip_set_pixel(strip, i, red, green, blue);
    }
}

esp_err_t led_strip_fill_with_solid(led_strip_handle_t strip, uint32_t red, uint32_t green, uint32_t blue)
{
    esp_err_t err = ESP_OK;
    for (int i = 0; i < NUM_LEDS; i++) {
        err = led_strip_set_pixel(strip, i, red, green, blue);
    }
    return err;
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
            err = led_strip_fill_with_solid(strip,state*10,state*10,state*20);
            break;
        case state_timer:
            err = led_strip_fill_with_solid(strip,state*10,state*10,state*20);
            break;
        case state_sync:
            err = led_strip_fill_with_solid(strip,state*10,state*10,state*20);
            break;
        case state_startup:
            err = led_strip_fill_with_solid(strip,state*10,state*10,state*20);
            break;
        case state_group_owner_active:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, 0, 0, 100);
            led_strip_set_usr_colors(strip);
            break;
        case state_group_owner_passive:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, 0, 100, 0);
            led_strip_set_usr_colors(strip);
            break;
        case state_listener_passive:
            err = led_strip_fill_with_solid(strip,0, 100, 0);
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