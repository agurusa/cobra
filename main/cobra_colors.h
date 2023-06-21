#pragma once
#include <stdint.h>

typedef struct cobra_colors_t{
    uint16_t lightness;
    uint16_t hue;
    uint16_t saturation;
}cobra_colors_t;

typedef struct cobra_colors_rgb_t{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
}cobra_colors_rgb_t;