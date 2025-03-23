#include <RCSwitch.h>
#include "lib.h"

char buffer[BUFFER_SIZE];
RCSwitch capture = RCSwitch();

void signal() {
  digitalWrite(STATUS_LED, HIGH);
  delay(350);
  digitalWrite(STATUS_LED, LOW);
  delay(350);
}

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