#ifndef LIB_H
#define LIB_H

#include <Ticker.h>

#define TRANS_PIN 14
#define RECV_PIN 12

#define BUTTON_COUNT 5
#define TRANS_COUNT 4
#define DEBOUNCE_DELAY 35
#define STATUS_LED 16
#define MAX_ANIMATION 4

#define CODE 8
#define CHN 4

#define BUFFER_SIZE 256

extern Ticker reset;

bool transmit = true;

volatile bool previousFlash = false;

volatile bool toggle_recv_trans = false;

volatile bool reset_OLED = false;

int dots = 0;

typedef void (*ISRFunction)();

extern ISRFunction buttonISRs[BUTTON_COUNT], handle[TRANS_COUNT];

static int buttonPins[BUTTON_COUNT] = {2, 15, 13, 0, 10};
extern volatile bool buttonState[BUTTON_COUNT];
extern volatile unsigned long lastDebounce[BUTTON_COUNT];

void attachReset();
void detachReset();

#endif