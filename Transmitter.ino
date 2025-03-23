#include <Ticker.h>
#include <RCSwitch.h>
#include "lib.h"

volatile bool buttonState[BUTTON_COUNT] = {false, false, false, false, false, false};
volatile unsigned long lastDebounce[BUTTON_COUNT] = {0, 0, 0, 0, 0, 0};

Ticker failsafe;
RCSwitch remote = RCSwitch();

const char* locked_prompt = "Door Locked. Unlock to continue!";

int opening = 0;
int closing = 0;
int locked = 1;

void IRAM_ATTR handleButtonPressPullup(int idx) {
  const unsigned long now = millis();
  if (now - lastDebounce[idx] > DEBOUNCE_DELAY) {
    buttonState[idx] = !digitalRead(buttonPins[idx]);
    lastDebounce[idx] = now;
  }
}

void IRAM_ATTR handleButtonPressPulldown(int idx) {
  const unsigned long now = millis();
  if (now - lastDebounce[idx] > DEBOUNCE_DELAY) {
    buttonState[idx] = digitalRead(buttonPins[idx]);
    lastDebounce[idx] = now;
  }
}

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
void IRAM_ATTR handleFnButton() { handleButton(5); }

void handleLock() { 
  locked = 1;
  opening = 0;
  closing = 0;
  failsafe.detach();
  display_msg("Door Locked!");
  remote.sendTriState("110110FF0010"); // Lock
}

void handleHalt() { 
  const char* msg = locked ? "Door Unlocked!" : "Door Halted!";
  locked = 0;
  opening = 0;
  closing = 0;
  failsafe.detach();
  display_msg(msg);
  remote.sendTriState("110110FF0100"); // Stop
}

void handleDown() { 
  if (locked) {
    display_msg(locked_prompt);
    return;
  }

  if (opening) {
    display_msg("Door is opening. Halt to continue!");
    return;
  }

  if (!closing) {
    failsafe.detach();
    failsafe.attach(10.0,  []() { closing = 0; });
    dots = 0;
  }
  closing = 1;
  remote.sendTriState("110110FF1000"); // Down
  display_cont_msg("Door Closing", ".", dots);
  dots = (dots + 1) % MAX_ANIMATION;
}

void handleUp() { 
  if (locked) {
    display_msg(locked_prompt);
    return;
  }

  if (closing) {
    display_msg("Door is closing. Halt to continue!");
    return;
  }

  if (!opening) {
    failsafe.detach();
    failsafe.attach(10.0,  []() { opening = 0; });
    dots = 0;
  }
  opening = 1;
  remote.sendTriState("110110FF0001"); // Up
  display_cont_msg("Door Opening", ".", dots);
  dots = (dots + 1) % MAX_ANIMATION;
}
  // Function pointer type for ISRs
ISRFunction buttonISRs[BUTTON_COUNT] = {handleLockButton, handleHaltButton, handleDownButton, handleUpButton, handleMenuButton, handleFnButton};
ISRFunction handle[TRANS_COUNT] = {handleLock, handleHalt, handleDown, handleUp};

void transmitter_handle() {
  for (int i = 0; i < TRANS_COUNT; i++) {
    if (buttonState[i]) {
      detachReset();
      reset_OLED = false;
      digitalWrite(STATUS_LED, HIGH);
      handle[i]();
      attachReset(5.0);
    } else digitalWrite(STATUS_LED, LOW);
  }
}