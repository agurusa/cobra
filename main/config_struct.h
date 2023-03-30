#pragma once
#include <stdint.h>

typedef struct config_info_t{
    uint16_t net_idx; /* NetKey Index */
    uint16_t app_idx; /* AppKey Index */
    uint8_t tid; /*transaction id*/
}config_info_t;



