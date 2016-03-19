/**
  Button
  button.cpp
  
  Implementation for the arduino Button class.
  @author Ken Barbour
  @version 0.1 3/17/2016
 */
#include "Button.h"

Button::Button(const int buttonPin, const int buttonMode) {
  pin = buttonPin;
  mode = buttonMode;
  lastState = buttonMode;
  lastDebounceTime = 0;
  pinMode(pin, INPUT);
}
bool Button::isPressed() {
  int inBtn = digitalRead(pin);
  if (inBtn != lastState)
    lastDebounceTime = millis();
  lastState = inBtn;
  return isStatePressed(inBtn);
}
bool Button::wasPressed() {
  return isStatePressed(lastState);
}
bool Button::stateChanged() {
  return (this->wasPressed() != this->isPressed());
}
bool Button::uniquePress() {
  return (this->stateChanged() && this->wasPressed());
}

/**
 * Determine if the provided state of the input pin is pressed
 * based on the mode of the button
 * @return bool
 */
bool Button::isStatePressed(const int state) {
  if (this->mode == MODE_PULLDOWN)
    return (state == HIGH);
  else return (state == LOW);
}
