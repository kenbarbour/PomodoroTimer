#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeLib.h>
#include "Button.h"

/*
 Pomodoro timer
 
 Organizes time spent working and break time into sprints and breaks.
 Press the START_BUTTON to start the sprint timer.  The first LED should
 flash, letting you know the timer has started. Complete a sprint by
 working for a full WORK_TIME and pressing the START_BUTTON once the first
 LED starts blinking again.  After a sprint,
 
 Components:
  * Green LED (x4)
  * Red LED
  * 10K ohm Resistor
  * 220 ohm Resistor (x5)
  
*/

const int START_BUTTON_PIN = 2;
int LED_PINS[] = {9,6,5,3,7}; // Array of LED pins (TOTAL_SPRINTS + 1)

//-- Timer configuration
// number of sprints before a big break
const int TOTAL_SPRINTS = 4;
// time in seconds of a work sprint
const int SPRINT_TIME = 25;
// time in seconds of a break
const int BREAK_TIME = 5;
// time in seconds of a big reward break
const int BIG_BREAK_TIME = 30;

const int FLASH_DELAY = 100;
const int FLASH_DELAY_QUICK = 25;

const int STATUS_BEFORE = 0; //  Before a sprint
const int STATUS_SPRINT = 1; // During a sprint
const int STATUS_AFTER = 2; // After a sprint
const int STATUS_BREAK = 3; // During a break
const int STATUS_BIG_BREAK = 4; // During a big break

const int DEBUG = 1;

// Declare vars
int currentSprint = 0; // current sprint (zero based)
int currentStatus = STATUS_BEFORE;
time_t sprintStartTime = 0;
Button* startButton;

void setup() {
  
  Serial.begin(9600);
  Serial.print("Initializing pins...\n");
  // initialize pins
  for (int i = 0; i <= TOTAL_SPRINTS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  startButton = new Button(START_BUTTON_PIN,Button::MODE_PULLDOWN);
  
  // flash LEDS
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j <= TOTAL_SPRINTS; j++)
      digitalWrite(LED_PINS[j],HIGH);
    delay(FLASH_DELAY);
    for (int j = 0; j <= TOTAL_SPRINTS; j++)
      digitalWrite(LED_PINS[j],LOW);
    delay(FLASH_DELAY);
  }
}

void loop() {
  if (startButton->uniquePress())
    onStartPressed();
    
  if (currentStatus == STATUS_SPRINT &&
    sprintStartTime + SPRINT_TIME >= now())
    onSprintComplete();
}

void onStartPressed() {
  Serial.print("Start pressed\n");
  delay(1000);
  switch (currentStatus) {
    case STATUS_BEFORE:
      onStartSprint();
      break;    
    case STATUS_SPRINT:
      break;    
    case STATUS_AFTER:
      break;    
    case STATUS_BREAK:
      break;    
    case STATUS_BIG_BREAK:
      break;    
  }
}


/**
 Starts the currentSprint if currentStatus is STATUS_BEFORE
 */
void onStartSprint() {
  
  Serial.print("Starting sprint\n");
  
  if (currentStatus != STATUS_BEFORE)
    return;
  
  // Illuminate completed sprints
  for (int i = 0; i < currentSprint; i++)
    digitalWrite(LED_PINS[i], HIGH);
    
  // Flash current sprint
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[currentSprint],HIGH);
    delay(FLASH_DELAY);
    digitalWrite(LED_PINS[currentSprint],LOW);
    delay(FLASH_DELAY);
  }
  
  currentStatus = STATUS_SPRINT;
  sprintStartTime = now();
}

/**
 Completes the currentSprint if currentStatus is STATUS_SPRINT by setting
 the currentStatus to STATUS_AFTER and incrementing the currentSprint
 */
void onSprintComplete() {
  
  Serial.print("Sprint complete\n");
  if (currentStatus != STATUS_SPRINT)
    return;
  currentSprint++;
  currentStatus = STATUS_AFTER;
}

/**
 Starts the break after a sprint if currentStatus is STATUS_AFTER by
 resetting the sprintStartTime to now and currentStatus to STATUS_BREAK
 */
void onStartBreak() {
  if (currentStatus != STATUS_AFTER)
    return;
    
  sprintStartTime = now();
  if (currentSprint >= TOTAL_SPRINTS)
    currentStatus = STATUS_BIG_BREAK;
  else
    currentStatus = STATUS_BREAK;
}

void onBreakComplete() {
  if (currentStatus != STATUS_BREAK)
    return;
  currentStatus = STATUS_BEFORE;
}
