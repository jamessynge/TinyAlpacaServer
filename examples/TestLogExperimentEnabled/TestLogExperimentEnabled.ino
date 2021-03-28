#ifndef TAS_ENABLE_CHECK
#define TAS_ENABLE_CHECK
#endif

#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif
#define TAS_ENABLED_VLOG_LEVEL 2

#include <Arduino.h>
#include <TinyAlpacaServer.h>

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

  LogExperimentTestFunction();
}

void loop() {}
