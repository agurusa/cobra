#ifndef _PERIODIC_TIMER_GUARD
#define _PERIODIC_TIMER_GUARD

#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cobra_button.h"
#include "cobra_leds.c"
#include "state_enum.h"
#include "cobra_queue.c"

const uint64_t PERIOD_MS = 5; /*read hardware every 5 ms*/
const char * TIMER_TAG = "TIMER";

const int CHECK_MSEC= 5; // read hardware every 5 msec

const int MODE_BUTTON_PIN = GPIO_NUM_3;
const int COMMS_BUTTON_PIN = GPIO_NUM_2;

typedef struct {
  bool comms_key_changed;
  bool comms_key_pressed;
  bool mode_key_changed;
  bool mode_key_pressed;
} timer_info_t;

// initialize button pins as input. Use the internal pullup resistor.
Button modeButton = {
  .type = button_type_mode, 
  .PIN = MODE_BUTTON_PIN, 
  .isPressed = false, 
  .wasPressed = false, 
  .pressTime = press_time_none,
  .pressCount = 0, 
  .debounce = {false,RELEASE_MSEC/CHECK_MSEC}
};

Button commsButton = {
  .type = button_type_comms, 
  .PIN = COMMS_BUTTON_PIN, 
  .isPressed = false, 
  .wasPressed = false, 
  .pressTime = press_time_none,
  .pressCount = 0, 
  .debounce = {false,RELEASE_MSEC/CHECK_MSEC}
};

void debounceButton(bool *key_changed, bool *key_pressed, Button *button)
{
  *key_changed = false;
  *key_pressed = button->debounce.debouncedKeyPressed;
  bool rawState = rawKeyPressed(button->PIN);
  if (rawState == button->debounce.debouncedKeyPressed) {
    // set the timer which allows a change from the current state
    if (button->debounce.debouncedKeyPressed) {
      button->debounce.count = RELEASE_MSEC/CHECK_MSEC;
    }
    else {
      button->debounce.count = PRESS_MSEC/CHECK_MSEC;
    }
  }
  else {
    // key changed- wait for state to become stable.
    button->debounce.count--;
    if(button->debounce.count == 0) {
      // timer expired - accept the change
      button->debounce.debouncedKeyPressed = rawState;
      *key_changed = true;
      *key_pressed = button->debounce.debouncedKeyPressed;
      // reset the timer
      if (button->debounce.debouncedKeyPressed) {
      button->debounce.count = RELEASE_MSEC/CHECK_MSEC;
      }
      else {
        button->debounce.count = PRESS_MSEC/CHECK_MSEC;
      }
    }
  }
}

void timer_isr_callback(void *args)
{
  timer_info_t *info = (timer_info_t *) args;
  debounceButton(&info->comms_key_changed, &info->comms_key_pressed, &commsButton);
  debounceButton(&info->mode_key_changed, &info->mode_key_pressed, &modeButton);
  modeButton.isPressed = info->mode_key_pressed;
  commsButton.isPressed = info->comms_key_pressed;
  if (info->mode_key_pressed){
    buttonPressed(&modeButton);
  }
  else if (info->mode_key_changed)
  {
    buttonReleased(&modeButton);
  }
  if (info->comms_key_pressed){
    buttonPressed(&commsButton);
  }
  else if (info->comms_key_changed) {
    buttonReleased(&commsButton);
  } 
};

void pressModeButton()
{
    cobra_process_t proc = modeButton.pressTime == press_time_long ? process_mode_button_press_long: process_mode_button_press_short;
    cobra_process_info_t proc_info = {
      .process = proc,
      .priority = high
    };
    add_process(proc_info);

}

void pressCommsButton()
{
    cobra_process_t proc = commsButton.pressTime == press_time_long ? process_comms_button_press_long : process_comms_button_press_short;
    cobra_process_info_t proc_info = {
      .process = proc,
      .priority = high
    };
    add_process(proc_info);
    
}

/* TESTING ONLY (todo: move to official test)*/
void test_process_queue(void *args)
{
  bool pressMode = false;
  while(1)
  {
    pressMode ? pressModeButton(): pressCommsButton();
    pressMode = !pressMode;
  }

}


void check_buttons(void *pvParameters)
{
    while (1)
    {
        if (modeButton.wasPressed) {
            pressModeButton();
            modeButton.wasPressed=false;
            modeButton.pressTime = press_time_none;
        }
        else if (commsButton.wasPressed)
        {
            pressCommsButton();
            commsButton.wasPressed=false;
            commsButton.pressTime = press_time_none;
        }
        vTaskDelay(50 / 1000);
    }
    
}

#endif