#include "lib.h"
#include <Ticker.h>

// OLED refresh scheduler object
Ticker reset;

void attachReset(float time) { reset.attach(time, reset_status); } // Schedule OLED display refresh

void detachReset() { reset.detach(); } // Disable OLED display refresh task