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

typedef struct {
  bool debouncedKeyPressed;
  int count;

}debounce_struct_t;

struct Button{
  const ButtonType type;
  const int PIN;
  bool isPressed;
  bool wasPressed;
  PressTime pressTime;
  int pressCount;
  debounce_struct_t debounce;
};
