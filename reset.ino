#include "lib.h"
#include <Ticker.h>

Ticker reset;

void attachReset(float time) { reset.attach(time, reset_status); }

void detachReset() { reset.detach(); }