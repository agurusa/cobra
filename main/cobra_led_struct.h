#pragma once

#include "led_strip.h"

typedef struct {
    led_strip_handle_t strip;
    uint32_t index;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
}led_struct_t;