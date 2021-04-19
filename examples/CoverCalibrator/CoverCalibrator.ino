#include <Arduino.h>

#include "src/server.h"

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(57600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }

  // Initialize networking and the server
  astro_makers::setup();
}

void loop() { astro_makers::loop(); }
