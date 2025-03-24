/**
  This Header file define all the constants and abstractions for the remote. Optimized for ESP8266
  Adjust if necessary for other platforms
*/

#ifndef LIB_H
#define LIB_H

#include <Ticker.h>
#include <RCSwitch.h>

/* RF Transceiver config */
#define TRANS_PIN 14 // RF Transmitter - GPIO14 (D5 on NodeMCU)
#define RECV_PIN 12 // RF Receiver - GPIO12 (D6 on NodeMCU)
/* End RF Transceiver config */

/* Buttons config */
#define BUTTON_COUNT 6 // Number of buttons (6/4)
#define TRANS_COUNT 4 // Number of buttons for transmitting (2 (pending)/4)
#define DEBOUNCE_DELAY 30 // Debounce window for buttons
#define LONG_PRESS 400 // Long press window for Fn button

static int buttonPins[BUTTON_COUNT] = {2, 15, 13, 0, 3, 10};
extern volatile bool buttonState[BUTTON_COUNT];
extern volatile unsigned long lastDebounce[BUTTON_COUNT];

// Abstract defintion for buttons Interrupt handlers
typedef void (*ISRFunction)();
extern ISRFunction buttonISRs[BUTTON_COUNT], handle[TRANS_COUNT];
/* End buttons config */

#define STATUS_LED 16 // Status LED - GPIO16 (D0 on NodeMCU)

#define MAX_ANIMATION 4 // For messages with animation in Transmit mode
int dots = 0; // Animation flag

/* RF Transmitter config */
#define PROTOCOL 1
#define PULSE_LENGTH 320
/* End RF Transmitter config */

/* RF Receiver config */
#define BUFFER_SIZE 256 // Output message buffer size
#define CODE 8 // Length of captured code (tri-state)
#define CHN 4 // Max number of channels
/* End RF Receiver config */

/* Menu config */
#define OPTIONS 3 // Number of menu options
#define OPTION_START_X 5 // X-axis starting point of option 0
#define OPTION_START_Y 29 // Y-axis starting point of option 0 

// Menu mode
typedef enum {
  TRANSMIT = 0,
  RECEIVE = 1,
  SETTING = 2,
  MENU = 3,
} Mode;

Mode mode = MENU;

// Menu options tracking
int st = 0, prev_st = st;
static String options[] = {"RF Remote", "RF Receiver", "Settings"};
/* End menu config */

// Ticker object for periodic task scheduling
extern Ticker reset, failsafe;

// rc-switch object for RF Transceiver modules
extern RCSwitch remote, capture;

// Flag for OLED display refresh
volatile bool reset_OLED = false;

int led_state = 0; // Status LED flag

// Fn button flags
volatile unsigned long pressStart = 0;
volatile bool longPress = false;

extern volatile bool tranceiverInit[2]; // Abstract for initial RF Transmit display state

// Abstract for OLED display refresh schedulers
void attachReset();
void detachReset();

#endif