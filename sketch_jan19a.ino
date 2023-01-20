#include <ezButton.h> // esp32 library to handle button debouncing and internal pullup resistor


// used to indicate which button was pressed
enum Button {
  mode,
  comms, 
  none
}

// used to indicate how long a button was pressed
enum PressTime {
  long,
  short
}

ezButton modeButton(21);
ezButton commsButton(22);

const int DEBOUNCE_TIME = 50; // ms
const int LONG_PRESS_TIME = 1000; // ms
Button lastPressedButton = Button.none; // used to track long button presses
unsigned int lastPressedTime = 0; // keeps track of how long a button has been pressed
int pressTime = 0; // difference between the last pressed time and the current time 

void setup() {
  Serial.begin(9600); // for debugging
  // initialize button pins as input. Use the internal pullup resistor.
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(commsButtonPin, INPUT_PULLUP);
  modeButton.setDebounceTime(DEBOUNCE_TIME);
  commsButton.setDebounceTime(DEBOUNCE_TIME);

}

void pressModeButton(PressTime pressTime){
  // if long press
  // power off if on, power on if off
  // else
  // change mode

}

void pressCommsButton(PressTime pressTime){
  // send out comms, depending on the current mode
}

void pressButton(Button btn, PressTime pressTime) {
  // perform the action associated with the button press
  if (btn == Button.mode) {
    pressModeButton(pressTime);    
  }
  else {
    pressCommsButton(pressTime);
  }
}

Button getButtonState()
{
  // returns the Button that is pressed. If no button is pressed, will return Button.none
  modeButtonState = modeButton.isPressed()
  commsButtonState = commsButton.isPressed()
  if (modeButton.isPressed()) {
    return Button.mode;
  }
  else if (commsButton.isPressed()) {
    return Button.comms;
  }
  else {
    return Button.none;
  }
}

  void loop() {
    // necessary button setup for ezbutton library
    modeButton.loop();
    commsButton.loop();
    // read the button state. 
    Button pressedButton = getButtonState();
    // if a button is currently pressed, see how long that button has been pressed.
    // if this is the first moment the button is pressed, start the clock.
    // otherwise, see if enough time has passed to count it as a "long press."
    if (pressedButton != Button.none) {
      if (pressedButton != lastPressedButton){
        lastPressedTime = millis(); 
      }
      else{
        pressTime = millis() - lastPressedTime;
        if (pressTime > LONG_PRESS_TIME){
          pressButton(pressedButton, PressTime.long);   
        }
      }
    }
    // if no buttons are pressed now, but a button *was* pressed without signaling
    // the long press time, consider this a short press of the previously pressed button.
    else if (pressedButton == Button.none && lastPressedButton != Button.none) {
      if (pressTime < LONG_PRESS_TIME){
        pressButton(lastPressedButton, PressTime.short);
      }
      // reset the PressTime after recognizing that we just released a long press.
      else {
        pressTime = 0;
        lastPressedTime = 0;     
      }
    }
    lastPressedButton = pressedButton
  
}
