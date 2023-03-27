#ifndef _COBRA_LEDS_GUARD
#define _COBRA_LEDS_GUARD

#include "esp_err.h"
#include "driver/gpio.h"
#include "led_strip.h"

#include "state_enum.h"

const char * LED_TAG = "LED";

led_strip_handle_t led_strip;


const int NUM_LEDS = 30;
const int LED_GPIO = GPIO_NUM_5;
// const int COMMS_LED_INDEX = 1;
const int COMMS_LED_INDEX = 0; /*debug only*/
const int STARTUP_LED_INDEX = 0;

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

esp_err_t fillBodyLeds(cobra_state_t state, led_strip_handle_t strip)
// fill body LEDs with the correct color combo
{
    esp_err_t err = ESP_OK;
    switch (state) {
        case state_music:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, 255,255,255);
            break;
        case state_timer:
            err = led_strip_set_pixel(strip, COMMS_LED_INDEX, 100, 255, 10);
            break;
        case state_sync:
            break;
        case state_startup:
            err = led_strip_set_pixel(strip, STARTUP_LED_INDEX, 10, 255, 255);

        default:
            break;
    }
    err = led_strip_refresh(strip);
    return err;
}

#endif