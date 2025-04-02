/**
  This Header file define all the constants and abstractions for the remote. Optimized for ESP8266
  Adjust if necessary for other platforms
*/

#ifndef LIB_H
#define LIB_H

#include <Ticker.h>
#include <RCSwitch.h>

/* RF Transceiver config */
#define TRANS_PIN 16 // RF Transmitter - GPIO14 (D5 on NodeMCU) (Switch to GPIO16 due to shorted GPIO14)
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

// Due to overvoltage and shorted GPIO14, status LED output is reassigned to GPIO1 (TX)
// Be cautious to use this pin as it is only suitable for output (Serial logging must be disable)
#define STATUS_LED 1 // Status LED - GPIO16 (D0 on NodeMCU)

#define MAX_ANIMATION 4 // For messages with animation in Transmit mode
int dots = 0; // Animation flag

/* RF Receiver config */
#define BUFFER_SIZE 256 // Output message buffer size
#define CODE 8 // Length of captured code (tri-state)
#define CHN 4 // Max number of channels
/* End RF Receiver config */

/* RF Transmitter config */
#define PROTOCOL 1
#define PULSE_LENGTH 320
#define TARGET "/code.txt"

static const char* org_code = "FFFFFFFF";
static const bool dual_chn = false;
char curr_code[CODE + 1];
bool curr_chn = false, chn_sel = false;
static const char* quad_channels[CHN] = {"0001", "0010", "0100", "1000"}; 
static const char* dual_channels[CHN / 2] = {"01", "10"}; 
extern char* code[CHN];

// Default status for Transmit mode
int opening = 0;
int closing = 0;
int locked = 0;
/* End RF Transmitter config */

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
static const char* options[] = {"RF Remote", "RF Receiver", "Settings"};
/* End menu config */

// Settings options tracking
#define NUM_X_START 15
#define NUM_Y_START 25
#define SPACE 3
#define BOX_WIDTH 9
#define BOX_HEIGHT 2
#define CHAR_HEIGHT 17

int st_opt = 0;
extern ISRFunction setting_handle[BUTTON_COUNT];
/* End Settings menu config */

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