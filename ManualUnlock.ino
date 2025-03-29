#include "lib.h"

/** 
The code in this repository is tailored exclusive for ESP8266. 
Can be configure for other platform but must meet certain requirement
- The platform has at least 11 usable GPIO pins (6-button variant) or 9 usable GPIO pins (4-button variant)
- At least 6 pin support Hardware Interrupt (For Arduino Uno, switch to polling configuration)
- One I2C channel (For the OLED display)
*/

/** 
  Store entrance state for RF Transmitter and RF Receiver mode
  These states will prevent constant reloading of OLED display on each loop
*/
volatile bool tranceiverInit[2] = {false, false};

// Deduce the operation mode from the indexes of the menu options
Mode resolve_mode(int st) {
  Mode selected = MENU;
  switch (st) {
    case 0:
      selected = TRANSMIT;
      break;
    case 1: 
      selected = RECEIVE;
      break;
    case 2: 
      selected = SETTING;
      break;
    case 3:
      break;
  }
  return selected;
}

// Deduce the highlighted options in Menu mode based on current operation mode
int resolve_opt(Mode mode) {
  int selected = -1;
  switch (mode) {
    case TRANSMIT:
      selected = 0;
      break;
    case RECEIVE: 
      selected = 1;
      break;
    case SETTING: 
      selected = 2;
      break;
    case MENU:
      selected = 3;
      break;
  }
  return selected;
}

void setup() {
  // Disable Serial debugging
  // Required for 6-button variant (GPIO3 - UART0 RX pin is used for Menu)
  Serial.end(); 

  pinMode(STATUS_LED, OUTPUT); // Set GPIO16 as pin out for status LED

  // Input and Interrupt trigger for the button pins
  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttonPins[i], INPUT);
    attachInterrupt(digitalPinToInterrupt(buttonPins[i]), buttonISRs[i], i < 4 ? CHANGE : FALLING);
  }

  // rc-switch options for RF transmit
  remote.enableTransmit(TRANS_PIN);
  remote.setProtocol(PROTOCOL);
  remote.setPulseLength(PULSE_LENGTH);

  // rc-switch options for RF receive
  capture.enableReceive(RECV_PIN);

  setup_broadcast_code();
  OLED_Setup();
}

void loop() {
  // Handle Fn button trigger (Dual mode: Single Press and Long Press (400ms window))
  if (lastDebounce[5] > 0) {
    if (digitalRead(buttonPins[5]) == LOW) {
        if (!longPress && (millis() - lastDebounce[5] > LONG_PRESS)) {
          longPress = true;
          led_state = !led_state;
          digitalWrite(STATUS_LED, led_state);
          delay(50);
        }
    } else {
      if (!longPress && millis() - lastDebounce[5] > DEBOUNCE_DELAY) {
        lastDebounce[5] = 0;
        buttonState[5] = true;
        delay(100);
      }
    }
  }

  // Handle Fn button event for Single Press mode (Only in Menu)
  // In Menu, pressing Fn button will bring the tranceiver to the selected mode
  if (buttonState[5] && mode != SETTING) {
    if (mode == MENU) {
      if (st == 0 || st == 1) tranceiverInit[st] = true; // Set up initial state for Transmit and Receive mode
      delay(100); // Avoid OLED refresh flickering
      mode = resolve_mode(st); 
      st = -1; // Reset menu option
      prev_st = st;
    }
    buttonState[5] = false; // Reset state
  }

  // Set up control for flashlight mode
  if (led_state) {
    digitalWrite(STATUS_LED, HIGH);
    delay(20);
  }

  // Handle Menu button event
  if (buttonState[4] && mode != SETTING) {
    // Enter Menu mode if not in Menu
    if (mode != MENU) {
      detachReset(); // Disable interrupt-based OLED refresh
      st = resolve_opt(mode);
      prev_st = st;
      mode = MENU;
      main_menu(st);
      delay(150);  // Avoid OLED refresh flickering
    }
    else {
      // Cycle through menu options
      st = (st + 1) % OPTIONS;
      main_menu(st);
    }
    buttonState[4] = false; // Reset state
  }

  if (reset_OLED) reset_display(); // Trigger OLED display refresh based on scheduled task

  // Handle Transmit mode
  if (mode == TRANSMIT) {
    // Handle initial state to avoid constant OLED display refresh during operation
    if (tranceiverInit[0]) {
      default_msg();
      tranceiverInit[0] = false;
    }
    transmitter_handle();
  }
  // Handle Receive mode
  else if (mode == RECEIVE) {
    if (tranceiverInit[1]) {
      default_msg();
      tranceiverInit[1] = false;
    }
    receiver_handle();
  }
  else if (mode == SETTING) {
    setting_menu(st_opt);
    if (!chn_sel) cursor(st_opt);
    else chn_cursor();
    settings_handler();
    delay(25);
  }
  // Handle Menu mode
  else if (mode == MENU) {
    if (prev_st != st) {
      main_menu(st);
      prev_st = st;
    }
  }
}
