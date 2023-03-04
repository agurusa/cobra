#ifndef _PERIODIC_TIMER_GUARD
#define _PERIODIC_TIMER_GUARD

#include "esp_timer.h"
#include "esp_log.h"

const uint64_t PERIOD_MS = 5; /*read hardware every 5 ms*/
const char * TIMER_TAG = "TIMER";

typedef struct {
  bool comms_key_changed;
  bool comms_key_pressed;
  bool mode_key_changed;
  bool mode_key_pressed;
} timer_info_t;


void debounceButton(bool *key_changed){
    *key_changed = false;
    if (!*key_changed)
    {
        ESP_LOGI(TIMER_TAG, "not");
    }
    else 
    {
        ESP_LOGE(TIMER_TAG, "true");
    }
};

void timer_isr_callback(void *args)
{
  int64_t current_time = esp_timer_get_time();
  timer_info_t *info = (timer_info_t *) args;
  debounceButton(&info->comms_key_changed);
//   debounceButton(&info->mode_key_changed, &info->mode_key_pressed, &modeButton);
//   debounceButton(&info->comms_key_changed, &info->comms_key_pressed, &commsButton);
//   modeButton.isPressed = info->mode_key_pressed;
//   commsButton.isPressed = info->comms_key_pressed;
//   if (info->mode_key_pressed){
//     buttonPressed(&modeButton);
//   }
//   else if (info->mode_key_changed)
//   {
//     buttonReleased(&modeButton);
//   }
//   if (info->comms_key_pressed){
//     buttonPressed(&commsButton);
//   }
//   else if (info->comms_key_changed) {
//     buttonReleased(&commsButton);
//   } 
};


#endif