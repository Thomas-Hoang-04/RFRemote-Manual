#include <Ticker.h>
#include <RCSwitch.h>
#include "lib.h"

// Store state and last debounce time for each button 
volatile bool buttonState[BUTTON_COUNT] = {false, false, false, false, false, false};
volatile unsigned long lastDebounce[BUTTON_COUNT] = {0, 0, 0, 0, 0, 0};

Ticker failsafe;
RCSwitch remote = RCSwitch();

const char* locked_prompt = "Door Locked. Unlock to continue!";

// For button on GPIO0 and GPIO2 (require HIGH on startup)
void IRAM_ATTR handleButtonPressPullup(int idx) {
  const unsigned long now = millis();
  bool state = mode == SETTING ? true : !digitalRead(buttonPins[idx]);
  if (now - lastDebounce[idx] > DEBOUNCE_DELAY) {
    buttonState[idx] = state;
    lastDebounce[idx] = now;
  }
}

void IRAM_ATTR handleButtonPressPulldown(int idx) {
  const unsigned long now = millis();
  bool state = mode == SETTING ? true : digitalRead(buttonPins[idx]);
  if (now - lastDebounce[idx] > DEBOUNCE_DELAY) {
    buttonState[idx] = state;
    lastDebounce[idx] = now;
  }
}

// For Menu button
void IRAM_ATTR handleButton(int idx) {
  const unsigned long now = millis();
  if (now - lastDebounce[idx] > DEBOUNCE_DELAY) {
    buttonState[idx] = true;
    lastDebounce[idx] = now;
  } 
}

void IRAM_ATTR handleLockButton() { handleButtonPressPullup(0); }
void IRAM_ATTR handleHaltButton() { handleButtonPressPulldown(1); }
void IRAM_ATTR handleDownButton() { handleButtonPressPulldown(2); }
void IRAM_ATTR handleUpButton() { handleButtonPressPullup(3); }
void IRAM_ATTR handleMenuButton() { handleButton(4); }
// Custom handler for Fn button
void IRAM_ATTR handleFnButton() { 
  const unsigned long now = millis();
  if (now - lastDebounce[5] < DEBOUNCE_DELAY) return;
  
  lastDebounce[5] = now;
  longPress = false;
}

void handleLock() { 
  locked = 1;
  opening = 0;
  closing = 0;
  failsafe.detach(); // Disable refresh-to-default task
  display_msg("Door Locked!");
  remote.sendTriState(code[1]); // Lock
}

void handleHalt() { 
  const char* msg = locked ? "Door Unlocked!" : "Door Halted!";
  locked = 0;
  opening = 0;
  closing = 0;
  failsafe.detach(); // Disable refresh-to-default task
  display_msg(msg);
  remote.sendTriState(code[2]); // Stop
}

void handleDown() { 
  if (locked) {
    display_msg(locked_prompt);
    return;
  }

  if (opening && !curr_chn) {
    display_msg("Door is opening. Halt to continue!");
    return;
  }

  if (!closing && !curr_chn) {
    failsafe.detach(); // Reset refresh-to-default task
    failsafe.attach(10.0,  []() { closing = 0; }); // Task to automatically reset target status
    dots = 0;
  }
  const char* prompt = curr_chn ? "Channel 1" : "Door Closing";
  if (!curr_chn) closing = 1;
  remote.sendTriState(curr_chn ? code[1] : code[3]); // Down
  display_cont_msg(prompt, ".", dots);
  dots = (dots + 1) % MAX_ANIMATION;
}

void handleUp() { 
  if (locked) {
    display_msg(locked_prompt);
    return;
  }

  if (closing && !curr_chn) {
    display_msg("Door is closing. Halt to continue!");
    return;
  }

  if (!opening && !curr_chn) {
    failsafe.detach(); // Reset refresh-to-default task
    failsafe.attach(10.0,  []() { opening = 0; }); // Task to automatically reset target status
    dots = 0;
  }
  const char* prompt = curr_chn ? "Channel 0" : "Door Opening";
  if (!curr_chn) opening = 1;
  remote.sendTriState(code[0]); // Up
  display_cont_msg(prompt, ".", dots);
  dots = (dots + 1) % MAX_ANIMATION;
}

// Store interrupt handlers for batch setup
ISRFunction buttonISRs[BUTTON_COUNT] = {handleLockButton, handleHaltButton, handleDownButton, handleUpButton, handleMenuButton, handleFnButton};

// Store transmit event handlers for batch processing
ISRFunction handle[TRANS_COUNT] = {handleLock, handleHalt, handleDown, handleUp};

void transmitter_handle() {
  for (int i = 0; i < TRANS_COUNT; i++) {
    if (curr_chn && i < 2) continue;
    if (buttonState[i]) {
      // Disable OLED refresh
      detachReset();
      reset_OLED = false;
      // Handle event correspondingly
      digitalWrite(STATUS_LED, HIGH);
      handle[i]();
      attachReset(5.0); // Task to refresh the OLED display
      digitalWrite(STATUS_LED, LOW);
      delay(150); // Avoid state debounce & flickering
    } 
  }
}