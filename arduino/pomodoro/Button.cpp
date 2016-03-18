#include "Button.h"
#include <Arduino.h>

Button::Button(int buttonPin, int buttonMode) {
  this->pin = buttonPin;
  this->mode = buttonMode;
  this->lastState = buttonMode;
  pinMode(this->pin, INPUT);
}
int Button::isPressed() {
  int inBtn = digitalRead(this->pin);
  this->lastState = inBtn;
  return this->readState(inBtn);
}
int Button::wasPressed() {
  return readState(lastState);
}
int Button::stateChanged() {
  return (this->wasPressed() != this->isPressed());
}
int Button::uniquePress() {
  return (this->stateChanged() && this->wasPressed());
}
int Button::readState(int state) {
  if (this->mode == BUTTON_MODE_PULLDOWN)
    return (state == HIGH);
  else return (state == LOW);
}
