#ifndef LIB_H
#define LIB_H

#include <Ticker.h>
#include <RCSwitch.h>

#define TRANS_PIN 14
#define RECV_PIN 12

#define BUTTON_COUNT 6
#define TRANS_COUNT 4
#define DEBOUNCE_DELAY 30
#define LONG_PRESS 400
#define STATUS_LED 16
#define MAX_ANIMATION 4

#define CODE 8
#define CHN 4

#define BUFFER_SIZE 256

#define OPTIONS 3
#define OPTION_START_X 5
#define OPTION_START_Y 29

extern Ticker reset, failsafe;

extern RCSwitch remote, capture;

typedef enum {
  TRANSMIT = 0,
  RECEIVE = 1,
  SETTING = 2,
  MENU = 3,
} Mode;

Mode mode = MENU;

volatile bool scnd_press = false;

volatile bool reset_OLED = false;

int led_state = 0;
volatile unsigned long pressStart = 0;
volatile bool longPress = false;

int dots = 0;

typedef void (*ISRFunction)();

extern ISRFunction buttonISRs[BUTTON_COUNT], handle[TRANS_COUNT];

static int buttonPins[BUTTON_COUNT] = {2, 15, 13, 0, 3, 10};
extern volatile bool buttonState[BUTTON_COUNT];
extern volatile unsigned long lastDebounce[BUTTON_COUNT];
extern volatile bool tranceiverInit[2];

void attachReset();
void detachReset();

int st = 0, prev_st = st;
static String options[] = {"RF Remote", "RF Receiver", "Settings"};

#endif