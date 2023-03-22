#ifndef _COBRA_LEDS_GUARD
#define _COBRA_LEDS_GUARD

#include "esp_err.h"
#include "led_strip.h"

#include "state_enum.h"

const char * LED_TAG = "LED";

led_strip_handle_t led_strip;

const int NUM_LEDS = 30;
const int LED_GPIO = GPIO_NUM_8;
const int COMMS_LED_INDEX = 0;

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

esp_err_t fillBodyLeds(cobra_state_t state)
// fill body LEDs with the correct color combo
{
    esp_err_t err = ESP_OK;
    switch (state) {
        case state_music:
            // fill_rainbow(leds, NUM_LEDS, CRGB::CRGB::BlueViolet);
            err = led_strip_set_pixel(led_strip, COMMS_LED_INDEX, 255,255,255);
            break;
        case state_timer:
            // fill_solid(leds, NUM_LEDS, CRGB::Red);
            err = led_strip_set_pixel(led_strip, COMMS_LED_INDEX, 100, 255, 10);
            break;
        case state_sync:
            // fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);
            break;
        default:
            // fill_solid(leds, NUM_LEDS, CRGB::White);
            break;
    }
    err = led_strip_refresh(led_strip);
    return err;
}

#endif