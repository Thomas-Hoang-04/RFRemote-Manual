#include <RCSwitch.h>
#include "lib.h"

RCSwitch remote = RCSwitch(), capture = RCSwitch();

char buffer[BUFFER_SIZE];

void receiver_handle() {
  if (capture.available()) {
    signal();
    const char* binCode = dec2binWzerofill(capture.getReceivedValue(), capture.getReceivedBitlength());
    capture.resetAvailable();
    const char* triState = bin2tristate(binCode);
    char code[CODE + 1], channel[CHN + 1];
    strncpy(code, triState, CODE);
    code[CODE] = '\0';
    strncpy(channel, triState + 8, CHN);
    channel[CHN] = '\0';
    snprintf(buffer, BUFFER_SIZE, "Received code: %s - Channel: %s", code, channel);
    display_msg(buffer);
    detachReset();
    attachReset(8);
  }
}

void setup() {
  Serial.begin(115200); // Activate Serial Terminal

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
  if (previousFlash != buttonState[4]) {
    digitalWrite(STATUS_LED, buttonState[4]);
    previousFlash = buttonState[4];
  }

  if (toggle_recv_trans) {
    signal();
    buttonState[2] = false;
    buttonState[3] = false;
    digitalWrite(STATUS_LED, LOW);
    const char* msg = transmit ? "Changing to receiver mode..." : "Changing to transmitter mode...";
    transmit = !transmit;
    display_msg(msg);
    delay(1500);
    default_msg();
    toggle_recv_trans = false;
  }

  if (reset_OLED) reset_display();
  if (transmit) transmitter_handle();
  else receiver_handle();
}
