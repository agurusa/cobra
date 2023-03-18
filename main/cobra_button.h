#pragma once

#include "driver/gpio.h"

const char * BUTTON_TAG = "BUTTON";

const int PRESS_MSEC =10; // stable time before registering pressed
const int RELEASE_MSEC =100; // stable time before registering released
const int LONG_PRESS_COUNT = 100; 

bool rawKeyPressed(int pin){
  return gpio_get_level(pin) == 0;
};

esp_err_t button_setup(int pin_num) {
  esp_err_t ret = ESP_OK;
  ret = gpio_set_pull_mode(pin_num,  GPIO_PULLUP_ONLY);
  ret = gpio_set_direction(pin_num, GPIO_MODE_INPUT);
  return ret;
};


// used to indicate which button was pressed
typedef enum {
  button_type_mode,
  button_type_comms, 
  button_type_none
} button_type_t;

// used to indicate how long a button was pressed
typedef enum {
  press_time_none,
  press_time_long,
  press_time_short
} press_time_t;

typedef struct {
  bool debouncedKeyPressed;
  int count;

}debounce_struct_t;

typedef struct {
  const button_type_t type;
  const int PIN;
  bool isPressed;
  bool wasPressed;
  press_time_t pressTime;
  int pressCount;
  debounce_struct_t debounce;
}Button;


void buttonPressed(Button *button)
{
  button->isPressed = true;
  button->pressCount++;
}

void buttonReleased(Button *button)
{
  button->wasPressed = true;
  if (button->pressCount > LONG_PRESS_COUNT)
  {
    button->pressTime = press_time_long;
  } 
  else
  {
    button->pressTime = press_time_short;
  }
  button->isPressed = false;
  button->pressCount = 0;
  
};
