#include <FastLED.h>
#include "driver/timer.h"
#include "state_enum.h"
#include "button.h"


const int TIMER_DIVIDER = 80; // hardware timer clock divider
const int TIMER_SCALE = 10000;
const int CHECK_MSEC= 5; // read hardware every 5 msec
const int PRESS_MSEC =10; // stable time before registering pressed
const int RELEASE_MSEC =100; // stable time before registering released
const int LONG_PRESS_COUNT = 100; 

const timer_config_t config = {
  .alarm_en = TIMER_ALARM_EN, //alarm enable
  .counter_en = TIMER_PAUSE, // counter enable
  .intr_type = TIMER_INTR_MAX, // interrupt type
  .counter_dir = TIMER_COUNT_UP, // counter direction
  .auto_reload = TIMER_AUTORELOAD_EN, // enable auto reload
  .divider = TIMER_DIVIDER // counter clock divider
}; //default clock source is APB

bool rawKeyPressed(int pin){
  return digitalRead(pin) == LOW;
}

// debug
template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

const int NUM_LEDS= 30;
const int LED_PIN = 15;


// initialize button pins as input. Use the internal pullup resistor.
debounce_struct_t modeDebounce{false,RELEASE_MSEC/CHECK_MSEC};
Button modeButton{
  .type = ButtonType::mode, 
  .PIN = MODE_BUTTON_PIN, 
  .isPressed = false, 
  .wasPressed = false, 
  .pressCount = 0, 
  .debounce = modeDebounce
};

debounce_struct_t commsDebounce{false,RELEASE_MSEC/CHECK_MSEC};
Button commsButton{
  .type = ButtonType::comms, 
  .PIN = COMMS_BUTTON_PIN, 
  .isPressed = false, 
  .wasPressed = false, 
  .pressCount = 0, 
  .debounce = commsDebounce
};

state_struct_t currentState {
  .state_index = 0,
  .state = AllStates[0],
};

CRGB leds[NUM_LEDS];

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

typedef struct {
  bool comms_key_changed;
  bool comms_key_pressed;
  bool mode_key_changed;
  bool mode_key_pressed;
} timer_info_t;



bool IRAM_ATTR timer_isr_callback(void *args)
{
  BaseType_t high_task_awoken = pdFALSE;

  // prepare event data that is sent back to the task
  uint64_t timer_counter_value;
  timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_counter_value);
  timer_info_t *info = (timer_info_t *) args;
  debounceButton(&info->mode_key_changed, &info->mode_key_pressed, &modeButton);
  debounceButton(&info->comms_key_changed, &info->comms_key_pressed, &commsButton);
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
  // send event dat aback to main program task
  // xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken)
  return high_task_awoken == pdTRUE;
}

void buttonReleased(Button *button)
{
  button->wasPressed = true;
  if (button->pressCount > LONG_PRESS_COUNT)
  {
    button->pressTime = PressTime::longPress;
  } 
  else
  {
    button->pressTime = PressTime::shortPress;
  }
  button->isPressed = false;
  button->pressCount = 0;
  
}

void buttonPressed(Button *button)
{
  button->isPressed;
  button->pressCount++;
}

void setup() {
  Serial.begin(115200); // for debugging
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  //initialize button pins
  pinMode(modeButton.PIN, INPUT_PULLUP);
  pinMode(commsButton.PIN, INPUT_PULLUP);

  // initialize timer
  timer_init(TIMER_GROUP_0 , TIMER_0, &config); // group, timer, configs
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0); //initialize counter val to 0
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_SCALE);
  timer_info_t *timer_info = (timer_info_t*)calloc(1, sizeof(timer_info_t));
  timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr_callback, timer_info, 0);
  timer_start(TIMER_GROUP_0, TIMER_0);


}


void pressModeButton(){
  // if long press
  if (modeButton.pressTime == PressTime::longPress){
  // power off if on, power on if off
    Serial.println("long press on press mode button: powering off/on");
  }
  else{
    updateState(currentState);
  }
}

void pressCommsButton(){
  // send out comms, depending on the current mode
  if (commsButton.pressTime == PressTime::longPress){
    Serial.println("long press on comms button");
  }
  else
  {
    Serial.println("short press on comms mode button: send out comms");
  }
}



void fillBodyLeds(CRGB *leds, State state)
// fill body LEDs with the correct color combo
{
  uint8_t starthue = beatsin8(5, 0, 255);
  uint8_t endhue = beatsin8(7, 0, 255);
  switch (state){
    case State::music:
      fill_rainbow(leds, NUM_LEDS, CRGB::CRGB::BlueViolet);
      break;
    case State::timer:
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      break;
    case State::sync:
      fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);
      break;
    default:
      fill_solid(leds, NUM_LEDS, CRGB::White);
      break;
  }
}

void updateState(state_struct_t &current_state)
{
  // change mode
  int next_index = current_state.state_index;
  if (current_state.state_index == sizeof(AllStates)/sizeof(AllStates[0])){
    next_index = 0;
  }
  else {
    next_index++;
  }
  current_state.state_index = next_index;
  current_state.state = AllStates[next_index];
}

  void loop() {
    // this speeds up the simulation
    delay(500);

    fillBodyLeds(leds, currentState.state);
    FastLED.show();
    if (modeButton.wasPressed) {
      pressModeButton();
      modeButton.wasPressed=false;
      modeButton.pressTime = PressTime::none;
    }
    else if (commsButton.wasPressed)
    {
      pressCommsButton();
      commsButton.wasPressed=false;
      commsButton.pressTime = PressTime::none;
    }
  
}
