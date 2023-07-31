#ifndef _COBRA_LEDS_GUARD
#define _COBRA_LEDS_GUARD

#include <stdlib.h>
#include <math.h>

#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/semphr.h"

#include "led_strip.h"
#include "cobra_led_struct.h"
#include "static_members.h"
#include "state_enum.h"

/* For when we start a background task that blinks the LED */
static int led_on = 0;
static uint16_t start_rgb = 0;
static uint32_t s_red = 0;
static uint32_t s_green = 0;
static uint32_t s_blue = 0;
static uint16_t s_hue = 0;
static uint16_t s_val = 10;
static bool fade_in = true;

const char *LED_TAG = "LED";

static led_strip_handle_t led_strip;

const int LED_GPIO = GPIO_NUM_5;
const int COMMS_LED_INDEX = 1;
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
    .mem_block_symbols = 0,            /* use default */
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
    .mem_block_symbols = 0,            /* use default */
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

void blink_led(void *args)
{

    led_struct_t *led_args = (led_struct_t *)args;
    // while(xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
    while (1)
    {
        if (led_on)
        {
            led_strip_set_pixel(led_strip, led_args->index, led_args->red, led_args->green, led_args->blue);
        }
        else
        {
            led_strip_set_pixel(led_strip, led_args->index, 0, 0, 0);
        }
        led_on = !led_on;
        led_strip_refresh(led_strip);
        vTaskDelay(10);
    }
}

void hsl_to_rgb(uint16_t hue, uint16_t lightness, uint16_t saturation, uint16_t *red, uint16_t *green, uint16_t *blue)
{
    uint16_t h = hue / 360;
    uint16_t s = saturation / 100;
    uint16_t l = lightness / 100;
    uint16_t chroma = (1 - abs(2 * l - 1)) * s;
    uint16_t X = chroma * (1 - abs((h / 60) % 2 - 1));
    uint16_t m = l - chroma / 2;
    uint16_t r = 0;
    uint16_t g = 0;
    uint16_t b = 0;
    if (0 <= h && h < 60)
    {
        r = chroma;
        g = X;
        b = 0;
    }
    else if (120 <= h && h < 180)
    {
        r = 0;
        g = chroma;
        b = X;
    }
    else if (180 <= h && h < 240)
    {
        r = 0;
        g = X;
        b = chroma;
    }
    else if (240 <= h && h < 300)
    {
        r = X;
        g = 0;
        b = chroma;
    }
    else if (300 <= h && h < 360)
    {
        r = chroma;
        g = 0;
        b = X;
    }
    else
    {
        ESP_LOGE(LED_TAG, "unknown h value: %u", h);
    }
    *red = round((r + m) * 255);
    *green = round((g + m) * 255);
    *blue = round((b + m) * 255);
}

void update_usr_color(cobra_colors_t color)
{
    update_usr_colors(color);
}

void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i)
    {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}

esp_err_t led_strip_pulse(led_strip_handle_t strip)
{
    esp_err_t err = ESP_OK;
    if (s_val >= 100){
        fade_in = false;
        start_rgb-=60;
    }
    else if(s_val <= 0){
        fade_in = true;
        start_rgb+=60;
    }
    if (fade_in){
        s_val+=30;
    }
    else{
        s_val-=20;
    }
    for (int j = 0; j < NUM_LEDS; j += 1)
    {
        // Build RGB values
        led_strip_hsv2rgb(start_rgb, 100, s_val, &s_red, &s_green, &s_blue);
        // Write RGB values to strip driver
        err = led_strip_set_pixel(strip, j, s_red, s_green, s_blue);
    }
    return err;
}

esp_err_t led_strip_rainbow(led_strip_handle_t strip){
    esp_err_t err = ESP_OK;
    for (int j = 0; j < NUM_LEDS; j += 1)
    {
        // Build RGB values
        s_hue = j * 360 / NUM_LEDS + start_rgb;
        led_strip_hsv2rgb(s_hue, 100, 100, &s_red, &s_green, &s_blue);
        // Write RGB values to strip driver
        err = led_strip_set_pixel(strip, j, s_red, s_green, s_blue);
    }
    start_rgb += 60;
    return err;
}

void led_strip_set_usr_colors(led_strip_handle_t strip)
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    for (int i = FIRST_USR_LED_INDEX; i < NUM_LEDS; i++)
    {
        cobra_colors_t color = get_usr_color(i);
        hsl_to_rgb(color.hue, color.lightness, color.saturation, &red, &green, &blue);
        led_strip_set_pixel(strip, i, red, green, blue);
    }
}

esp_err_t led_strip_fill_with_solid(led_strip_handle_t strip, uint32_t red, uint32_t green, uint32_t blue)
{
    esp_err_t err = ESP_OK;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        err = led_strip_set_pixel(strip, i, red, green, blue);
    }
    return err;
}

esp_err_t fillBodyLeds(cobra_state_t state, led_strip_handle_t strip)
// fill body LEDs with the correct color combo
{
    esp_err_t err = ESP_OK;
    switch (state)
    {
    case state_music:
        err = led_strip_rainbow(strip);
        break;
    case state_timer:
        err = led_strip_pulse(strip);
        break;
    case state_sync:
        err = led_strip_fill_with_solid(strip, state * 10, state * 10, state * 20);
        break;
    case state_startup:
        err = led_strip_fill_with_solid(strip, state * 10, state * 10, state * 20);
        break;
    case state_locator:
        err = led_strip_fill_with_solid(strip, state * 10, state * 10, state * 20);
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
        err = led_strip_set_pixel(strip, COMMS_LED_INDEX, 0, 100, 0);
        led_strip_set_usr_colors(strip);
        break;
    case state_listener_active:
        led_strip_set_pixel(strip, COMMS_LED_INDEX, 100, 0, 0);
        led_strip_set_usr_colors(strip);
        break;
    default:
        break;
    }
    err = led_strip_refresh(strip);
    return err;
}

#endif