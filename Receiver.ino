#include <RCSwitch.h>
#include "lib.h"

// Store system output message
char buffer[BUFFER_SIZE];

// Initialize rc-switch object for RF Receiver
RCSwitch capture = RCSwitch();

// LED flashing
void signal() {
  digitalWrite(STATUS_LED, HIGH);
  delay(350);
  digitalWrite(STATUS_LED, LOW);
  delay(350);
}

// Handle RF signals capture (Interrupt-based)
void receiver_handle() {
  if (capture.available()) {
    signal();
    const char* binCode = dec2binWzerofill(capture.getReceivedValue(), capture.getReceivedBitlength()); // Convert received decimal value to binary format (See rc-switch exmaples)
    capture.resetAvailable(); // Reset receiver for next incoming signal
    const char* triState = bin2tristate(binCode); // Convert binary format to tri-state format (See rc-switch exmaples)
    // Format an output the captured result
    char code[CODE + 1], channel[CHN + 1];
    strncpy(code, triState, CODE);
    code[CODE] = '\0';
    strncpy(channel, triState + 8, CHN);
    channel[CHN] = '\0';
    snprintf(buffer, BUFFER_SIZE, "Received code: %s - Channel: %s", code, channel);
    display_msg(buffer);

    // Schedule task to automatically refresh the display for next status 
    detachReset();
    attachReset(8);
  }
}

// Convert binary code to tri-state code
static const char* bin2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

// Convert captured decimal value to binary code
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}