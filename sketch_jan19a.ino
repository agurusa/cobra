#include <FastLED.h>

// used to indicate which button was pressed
enum class ButtonType {
  mode,
  comms, 
  none
};

// used to indicate how long a button was pressed
enum class PressTime {
  none,
  longPress,
  shortPress
};

struct Button{
  const ButtonType type;
  const int PIN;
  bool wasPressed;
  PressTime pressTime;
};

enum class State{
  music,
  timer,
  sync,
};

// debug
template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

const int MODE_BUTTON_PIN = 23;
const int COMMS_BUTTON_PIN = 22;
const int NUM_LEDS= 30;
const int LED_PIN = 15;


// initialize button pins as input. Use the internal pullup resistor.
Button modeButton{ButtonType::mode, MODE_BUTTON_PIN, false};
Button commsButton{ButtonType::comms, COMMS_BUTTON_PIN, false};
CRGB leds[NUM_LEDS];

const int DEBOUNCE_TIME = 50; // ms
const int LONG_PRESS_TIME = 1000; // ms
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;
unsigned long lastPressedTime = 0; // keeps track of how long a button has been pressed
unsigned long lastReleaseTime = 0;
unsigned long lastButtonChange = 0;
int pressDelta = 0; // difference between the last pressed time and the current time 
State state = State::music; //initialize to the music state
PressTime pressTime = PressTime::shortPress; // to be passed to the pressButton functions


// void IRAM_ATTR mode_isr(){
//   // the first moment we read a button push
//   if (millis() - lastPressedTime > DEBOUNCE_TIME)
//   {
//     if(digitalRead(MODE_BUTTON_PIN) == LOW){
//       lastPressedTime = millis();
//       Serial.println("button pin low");
//     }
//     else {
//       if (millis() - lastPressedTime > LONG_PRESS_TIME){
//         Serial.println("should be long press");
//         modeButton.pressTime = PressTime::longPress;
//       }
//       else {
//         Serial.println("should be short press");
//         modeButton.pressTime = PressTime::shortPress;
//       }
//       modeButton.wasPressed = true;
//       lastPressedTime = 0;
//     }
//   }
// }
int num = 0;
void IRAM_ATTR mode_isr(){
  if (millis() - lastButtonChange > DEBOUNCE_TIME)
  {
    lastButtonChange = millis();
    if (digitalRead(modeButton.PIN) == LOW && !modeButton.wasPressed)
    {
      lastPressedTime = millis();
      Serial.println("pressed");
    }
    else if (!modeButton.wasPressed){
      Serial.println("Pressing now...");
      modeButton.wasPressed = true;
      // if (millis() - lastPressedTime > LONG_PRESS_TIME){
      //   modeButton.pressTime = PressTime::longPress;
      // }
      // else {
      //   modeButton.pressTime = PressTime::shortPress;
      // }
      // num ++;
    }
  }
  // changes from high to low (pressed)
  // changes from low to high (released)
}


void IRAM_ATTR comms_isr(){
  if (digitalRead(COMMS_BUTTON_PIN) == LOW && lastPressedTime == 0) {
    lastPressedTime = millis();
  }
  else if (millis() - lastPressedTime > DEBOUNCE_TIME && digitalRead(COMMS_BUTTON_PIN) == HIGH) {
    commsButton.wasPressed = true;
    lastPressedTime = 0;
    if (millis() - lastPressedTime > LONG_PRESS_TIME) {
      commsButton.pressTime = PressTime::longPress;
    }
    else {
      commsButton.pressTime = PressTime::shortPress;
    }
  }
}

void setup() {
  Serial.begin(115200); // for debugging
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(COMMS_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(MODE_BUTTON_PIN, mode_isr, CHANGE);
  attachInterrupt(COMMS_BUTTON_PIN, comms_isr, CHANGE);

}


void pressModeButton(){
  // if long press
  if (modeButton.pressTime == PressTime::longPress){
  // power off if on, power on if off
    Serial.println("long press on press mode button: powering off/on");
  }
  else{
  // change mode
    Serial.println("short press on press mode button: changing mode");
  }
}

void pressCommsButton(){
  // send out comms, depending on the current mode
  if (commsButton.pressTime == PressTime::longPress){
    Serial.println("long press on comms mode button");
  }
  else
  {
    Serial.println("short press on comms mode button: send out comms");
  }
}

// void pressButton() {
//   pressDelta = millis() - lastPressedTime;
//     // perform the action associated with the button press
//     Button pressedButton = getButtonState();
//     if (pressedButton != Button::none) {
//       if (pressedButton != lastPressedButton){
//         lastPressedTime = millis(); 
//       }
//     }
//     else if (pressedButton == Button::none && lastPressedButton != Button::none) {
//         if (pressDelta < LONG_PRESS_TIME){
//           pressButton();
//         }
//         // This must be a long press. Run the long press for the previously pressed button.
//         // reset the PressTime after recognizing that we just released a long press.
//         else {
//           pressButton();
//           pressDelta = 0;
//           lastPressedTime = 0;     
//         }
//       }
//       lastPressedButton = pressedButton;

  
//   if (lastPressedButton == Button::mode) {
//     pressModeButton(pressTime);    
//   }
//   else {
//     pressCommsButton(pressTime);
//   }
// }

// Button getButtonState()
// {
//   // returns the Button that is pressed. If no button is pressed, will return Button.none
//   // note: we can't use isPressed() because that only relays the *change* between HIGH and LOW
//   // states. 
//   bool modeButtonPressed = digitalRead(MODE_BUTTON_PIN) == LOW;
//   bool commsButtonPressed = digitalRead(COMMS_BUTTON_PIN) == LOW;
//   if (modeButtonPressed) {
//     return Button::mode;
//   }
//   else if (commsButtonPressed) {
//     return Button::comms;
//   }
//   else {
//     return Button::none;
//   }
// }



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

void updateState(State &state, State next_state)
{
  state = next_state;
}

State allStates[3] = {State::music, State::timer, State::sync};
int state_index = 0;

  void loop() {
    // this speeds up the simulation
    delay(500);
    updateState(state, allStates[state_index]);
    if (state_index == (sizeof(allStates)/sizeof(allStates[0])) - 1){
      state_index = 0;
    }
    else {
      state_index ++;
    }
    fillBodyLeds(leds, state);
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
      modeButton.pressTime = PressTime::none;
    }



    // necessary button setup for ezbutton library
    // modeButton.loop();
    // commsButton.loop();
    // read the button state. 
    // Button pressedButton = getButtonState();
    // if a button is currently pressed, see how long that button has been pressed.
    // if this is the first moment the button is pressed, start the clock.
    // if (pressedButton != Button::none) {
    //   if (pressedButton != lastPressedButton){
    //     lastPressedTime = millis(); 
    //   }
    // }
    // if no buttons are pressed now, but a button *was* pressed without signaling
    // the long press time, consider this a short press of the previously pressed button.
    // else if (pressedButton == Button::none && lastPressedButton != Button::none) {
    //   pressDelta = millis() - lastPressedTime;
    //   if (pressDelta < LONG_PRESS_TIME){
    //     pressButton();
    //   }
    //   // This must be a long press. Run the long press for the previously pressed button.
    //   // reset the PressTime after recognizing that we just released a long press.
    //   else {
    //     pressButton();
    //     pressDelta = 0;
    //     lastPressedTime = 0;     
    //   }
    // }
    // lastPressedButton = pressedButton;
  
}
