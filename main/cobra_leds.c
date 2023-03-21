#ifndef _COBRA_LEDS_GUARD
#define _COBRA_LEDS_GUARD

#include "led_strip_interface.h"
#include "state_enum.h"

void fillBodyLeds(cobra_state_t state)
// fill body LEDs with the correct color combo
{
    switch (state) {
        case state_music:
            // fill_rainbow(leds, NUM_LEDS, CRGB::CRGB::BlueViolet);
            break;
        case state_timer:
            // fill_solid(leds, NUM_LEDS, CRGB::Red);
            break;
        case state_sync:
            // fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);
            break;
        default:
            // fill_solid(leds, NUM_LEDS, CRGB::White);
            break;
    }
}

#endif