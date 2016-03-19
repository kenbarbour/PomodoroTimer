/**
  Button
  button.h
  
  This Button class is for handling hardware buttons in Arduino.
  @author Ken Barbour
  @version 0.1 3/17/2016
 */
#ifndef Arduino_h
#include <Arduino.h>
#endif

class Button {
  uint8_t pin;
  uint8_t mode;
  uint8_t lastState;
  
  public:
    /**
     * Signifies that the button is normally HIGH (pulled up),
     * usually by connecting input to +5V with a 10K resistor.
     */
    const static unsigned int MODE_PULLUP = HIGH;
    
    /**
     * Signifies that the button is normally LOW (pulled down)
     * usually by connecting input to GND with a 10K resistor.
     */
    const static unsigned int MODE_PULLDOWN = LOW;
    
    const static unsigned long DEBOUNCE_DELAY = 50;
  
    /**
     * Button class constructor
     * @param int pin
     * @param int mode either MODE_PULLUP or MODE_PULLDOWN
     */
    Button(int, int);
    
    /**
     * Returns true if the button is pressed
     * @return int true (1) or false (0)
     */
    bool isPressed();
    
    /**
     * Returns true if the button was pressed the last time isPressed() was called
     * @return int true (1) or false (0)
     */
    bool wasPressed();
    
    /**
     * Returns true if the state of the button (pressed or not pressed) has changed since
     * isPressed() or stateChanged() was called
     * @return int true (1) or false (0)
     */
    bool stateChanged();
    
    /**
     * Checks for stateChanged() and if that state change was a button press (wasPressed())
     * @return int true (1) or false (0)
     */
    bool uniquePress();
    
  private:
    unsigned long lastDebounceTime;
    bool isStatePressed(int);
};
