#include "lib.h"

volatile bool tranceiverInit[2] = {false, false};

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
  Serial.end(); // Activate Serial Terminal

  pinMode(STATUS_LED, OUTPUT); // Define status LED pin out

  for (int i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttonPins[i], INPUT);
    attachInterrupt(digitalPinToInterrupt(buttonPins[i]), buttonISRs[i], i < 4 ? CHANGE : FALLING);
  }

  remote.enableTransmit(TRANS_PIN);
  remote.setProtocol(1);
  remote.setPulseLength(320);

  capture.enableReceive(RECV_PIN);
  OLED_Setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (buttonState[5] && mode == MENU) {
    if (st == 0 || st == 1) tranceiverInit[st] = true;
    delay(150);
    mode = resolve_mode(st);
    st = -1;
    prev_st = st;
    buttonState[5] = false;
  }

  if (buttonState[4]) {
    if (mode != MENU) {
      detachReset();
      st = resolve_opt(mode);
      prev_st = st;
      mode = MENU;
      main_menu(st);
      delay(250);
    }
    else {
      st = (st + 1) % OPTIONS;
      main_menu(st);
    }
    buttonState[4] = false;
  }

  if (reset_OLED) reset_display();

  if (mode == TRANSMIT) {
    if (tranceiverInit[0]) {
      default_msg();
      tranceiverInit[0] = false;
    }
    transmitter_handle();
  }
  else if (mode == RECEIVE) {
    if (tranceiverInit[1]) {
      default_msg();
      tranceiverInit[1] = false;
    }
    receiver_handle();
  }
  else if (mode == SETTING) {}
  else if (mode == MENU) {
    if (prev_st != st) {
      main_menu(st);
      prev_st = st;
    }
  }
}
