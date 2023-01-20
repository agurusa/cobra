#include <ezButton.h> // esp32 library to handle button debouncing and internal pullup resistor


// used to indicate which button was pressed
enum class Button {
  mode,
  comms, 
  none
};

// used to indicate how long a button was pressed
enum class PressTime {
  longPress,
  shortPress
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

// initialize button pins as input. Use the internal pullup resistor.
ezButton modeButton(MODE_BUTTON_PIN);
ezButton commsButton(COMMS_BUTTON_PIN);

const int DEBOUNCE_TIME = 50; // ms
const int LONG_PRESS_TIME = 1000; // ms
Button lastPressedButton = Button::none; // used to track long button presses
unsigned int lastPressedTime = 0; // keeps track of how long a button has been pressed
int pressTime = 0; // difference between the last pressed time and the current time 

void setup() {
  Serial.begin(9600); // for debugging
  modeButton.setDebounceTime(DEBOUNCE_TIME);
  commsButton.setDebounceTime(DEBOUNCE_TIME);

}

void pressModeButton(PressTime pressTime){
  // if long press
  if (pressTime == PressTime::longPress){
  // power off if on, power on if off
    Serial.println("long press on press mode button: powering off/on");
  }
  else{
  // change mode
    Serial.println("short press on press mode button: changing mode");
  }


}

void pressCommsButton(PressTime pressTime){
  // send out comms, depending on the current mode
  if (pressTime == PressTime::longPress){
    Serial.println("long press on comms mode button");
  }
  else
  {
    Serial.println("short press on comms mode button: send out comms");
  }
}

void pressButton(Button btn, PressTime pressTime) {
  // perform the action associated with the button press
  if (btn == Button::mode) {
    pressModeButton(pressTime);    
  }
  else {
    pressCommsButton(pressTime);
  }
}

Button getButtonState()
{
  // returns the Button that is pressed. If no button is pressed, will return Button.none
  // note: we can't use isPressed() because that only relays the *change* between HIGH and LOW
  // states. 
  bool modeButtonPressed = modeButton.getState() == 0;
  bool commsButtonPressed = commsButton.getState() == 0;
  if (modeButtonPressed) {
    return Button::mode;
  }
  else if (commsButtonPressed) {
    return Button::comms;
  }
  else {
    return Button::none;
  }
}

  void loop() {
    // this speeds up the simulation
    delay(10);
    // necessary button setup for ezbutton library
    modeButton.loop();
    commsButton.loop();
    // read the button state. 
    Button pressedButton = getButtonState();
    // if a button is currently pressed, see how long that button has been pressed.
    // if this is the first moment the button is pressed, start the clock.
    if (pressedButton != Button::none) {
      if (pressedButton != lastPressedButton){
        lastPressedTime = millis(); 
      }
    }
    // if no buttons are pressed now, but a button *was* pressed without signaling
    // the long press time, consider this a short press of the previously pressed button.
    else if (pressedButton == Button::none && lastPressedButton != Button::none) {
      pressTime = millis() - lastPressedTime;
      if (pressTime < LONG_PRESS_TIME){
        pressButton(lastPressedButton, PressTime::shortPress);
      }
      // This must be a long press. Run the long press for the previously pressed button.
      // reset the PressTime after recognizing that we just released a long press.
      else {
        pressButton(lastPressedButton, PressTime::longPress);
        pressTime = 0;
        lastPressedTime = 0;     
      }
    }
    lastPressedButton = pressedButton;
  
}
