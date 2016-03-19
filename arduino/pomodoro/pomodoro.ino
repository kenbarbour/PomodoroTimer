/*
 Pomodoro timer
 
 Organizes time spent working and break time into sprints and breaks.
 Press the button to start the sprint timer.  The first LED should
 flash, letting you know the timer has started. Complete a sprint by
 working for a full SPRINT_TIME and pressing the button once the first
 LED starts blinking again.
 
 Components:
  * Green LED (x4)
  * Red LED
  * 10K ohm Resistor
  * 220 ohm Resistor (x5)
  
*/
#define DEBUG_POMODORO 1
#ifdef DEBUG_POMODORO
#include <SoftwareSerial.h>
#endif

const int BUTTON_PIN = 2;
int LED_PINS[] = {9,6,5,3,7}; // Array of LED pins (TOTAL_SPRINTS + 1)

//-- Timer configuration
const int TOTAL_SPRINTS = 4;        // number of sprints before a big break
const int SPRINT_TIME = 25*60000;   // millisecs of a work sprint
const int BREAK_TIME = 5*60000;     // millisecs of a break
const int BIG_BREAK_TIME = 30*60000;// millisecs of a big reward break

const int FLASH_DELAY = 100;        // millisec delay b/w LED toggles during flashing
const int FLASH_DELAY_QUICK = 50;   // millisec delay b/w LED toggles during quick flashing
const int DEBOUNCE_DELAY = 50;      // min millisec delay b/w valid button state changes

//-- Pomodoro States
const int STATUS_BEFORE = 0;        // Before a sprint
const int STATUS_SPRINT = 1;        // During a sprint
const int STATUS_AFTER = 2;         // After a sprint
const int STATUS_BREAK = 3;         // During a break
const int STATUS_BIG_BREAK = 4;     // During a big break

// Declare Globals
int currentSprint = 0;              // current sprint (zero based)
int currentStatus = STATUS_BEFORE;  // current pomodoro state
unsigned long sprintStartTime = 0;  // time (millisecs) current sprint/break started
unsigned long lastDebounceTime = 0; // time (millisecs) button state last changed
int buttonState = LOW;              // current state of button
int lastButtonState = LOW;          // state of button last time it was checked
unsigned long lastFlashTime = 0;    // time (millisecs) flashing LEDs were last toggled
unsigned long lastFlashState = LOW; // pin state of flashing LEDs when they were last toggled

// function prototypes
void updateSprintLEDs();
void handleButton();
void handleState();
void debug(char*);

void setup() {
  
  #ifdef DEBUG_POMODORO
  Serial.begin(9600);
  #endif
  debug("Init");
  // initialize pins
  for (int i = 0; i <= TOTAL_SPRINTS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  pinMode(BUTTON_PIN, INPUT);
  
  // flash LEDS
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j <= TOTAL_SPRINTS; j++)
      digitalWrite(LED_PINS[j],HIGH);
    delay(FLASH_DELAY_QUICK);
    for (int j = 0; j <= TOTAL_SPRINTS; j++)
      digitalWrite(LED_PINS[j],LOW);
    delay(FLASH_DELAY);
  }
  debug("...done\n");
}

void loop() {
  handleButton();
  handleState();
}

void onStartPressed() {
  debug("button!\n");
  switch (currentStatus) {
    case STATUS_BEFORE:
      startSprint();
      break;    
    case STATUS_SPRINT:
      debug("Restart\n");
      restartSprint();
      break;    
    case STATUS_AFTER:
      startBreak();
      break;    
    case STATUS_BREAK:
    case STATUS_BIG_BREAK:
      debug("Skipping\n");
      breakComplete();
      break;    
  }
}


/**
 Starts the currentSprint if currentStatus is STATUS_BEFORE
 */
void startSprint() {
  
  debug("Starting sprint\n");
  
  if (currentStatus != STATUS_BEFORE)
    return;
    
  // Flash current sprint
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[currentSprint],HIGH);
    delay(FLASH_DELAY);
    digitalWrite(LED_PINS[currentSprint],LOW);
    delay(FLASH_DELAY);
  }
  
  setStatus(STATUS_SPRINT);
  sprintStartTime = millis();
}

void restartSprint() {
  debug("Restarting sprint\n");
  setStatus(STATUS_BEFORE);
}

/**
 Completes the currentSprint if currentStatus is STATUS_SPRINT by setting
 the currentStatus to STATUS_AFTER and incrementing the currentSprint
 */
void sprintComplete() {
  debug("Sprint complete\n");
  if (currentStatus != STATUS_SPRINT)
    return;
  currentSprint++;
  setStatus(STATUS_AFTER);
  updateSprintLEDs();
}

/**
 Starts the break after a sprint if currentStatus is STATUS_AFTER by
 resetting the sprintStartTime to now and currentStatus to STATUS_BREAK
 */
void startBreak() {
  if (currentStatus != STATUS_AFTER)
    return;
    
  digitalWrite(LED_PINS[TOTAL_SPRINTS],HIGH);
    
  sprintStartTime = millis();
  if (currentSprint >= TOTAL_SPRINTS)
    currentStatus = STATUS_BIG_BREAK;
  else
    setStatus(STATUS_BREAK);
}

void breakComplete() {
  if (currentStatus != STATUS_BREAK && currentStatus != STATUS_BIG_BREAK)
    return;
  digitalWrite(LED_PINS[TOTAL_SPRINTS],LOW);
  if (currentStatus == STATUS_BIG_BREAK) {
    currentSprint = 0;
    updateSprintLEDs();
  }
  setStatus(STATUS_BEFORE);
}

void setStatus(int newStatus) {
  switch (newStatus) {
    case STATUS_BEFORE:
      debug("BEFORE");
      break;
    case STATUS_SPRINT:
      debug("SPRINT");
      break;
    case STATUS_AFTER:
      debug("AFTER");
      break;
    case STATUS_BREAK:
      debug("BREAK");
      break;
  }
  debug("\n");
  currentStatus = newStatus;
}

/**
 * Checks if button has been pressed and calls onStartPressed() if true
 */
void handleButton() {
  int reading = digitalRead(BUTTON_PIN); // read button state
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // reading is older than debounce delay, it is valid
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == HIGH)
        onStartPressed();
    }
  }
  lastButtonState = reading;
}

/**
 *  Handle any logic that occurs on a loop for each state including flashing LEDS,
 *  calling other handlers (except button presses)
 */
void handleState() {
  switch (currentStatus) {
    default:
    case STATUS_BEFORE: // waiting on user input; flash current sprint led    
      if ((millis() - lastFlashTime) > FLASH_DELAY_QUICK) {
        int currentSprintPin = LED_PINS[currentSprint];
        lastFlashState = !lastFlashState;
        digitalWrite(currentSprintPin, lastFlashState);
        lastFlashTime = millis();
      }
      break;
    case STATUS_SPRINT:
      if ((millis() - sprintStartTime) > SPRINT_TIME) {
        sprintComplete();
      }
    break;
    case STATUS_AFTER:
      if ((millis() - lastFlashTime) > FLASH_DELAY_QUICK) {
        lastFlashState = !lastFlashState;
        digitalWrite(LED_PINS[TOTAL_SPRINTS], lastFlashState);
        lastFlashTime = millis();
      }
    break;
    case STATUS_BREAK:
      if ((millis() - sprintStartTime) > BREAK_TIME) {
        breakComplete();
      }
    break;
    case STATUS_BIG_BREAK:
      if ((millis() - sprintStartTime) > BIG_BREAK_TIME) {
        breakComplete();
      }
    break;
  }
}

void updateSprintLEDs() {
  for (int i = 0; i < TOTAL_SPRINTS; i++) {
    int value = (i < currentSprint) ? HIGH : LOW;
    digitalWrite(LED_PINS[i],value);
  }
}

void debug(const char* msg) {
  #ifdef DEBUG_POMODORO
  Serial.print(msg);
  #endif
}
