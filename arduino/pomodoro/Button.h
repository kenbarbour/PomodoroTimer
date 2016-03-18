/*
 Button
 */
const int BUTTON_MODE_PULLUP = 0;
const int BUTTON_MODE_PULLDOWN = 1;

class Button {
  int pin;
  int mode;
  int lastState;
  
  public:
    Button(int, int);
    int isPressed();
    int wasPressed();
    int stateChanged();
    int uniquePress();
    
  private:
    int readState(int);
};
