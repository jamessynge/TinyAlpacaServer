#include <Arduino.h>

#include "src/server.h"

const auto g_mcusr = MCUSR;

void setup() {
  // *Attempt* to learn why the microcontroller started/restarted executing the
  // sketch. To learn more, see:
  //   https://forum.arduino.cc/t/how-to-distinguish-between-reset-and-real-power-loss/239738
  const auto mcusr = MCUSR;
  // Clear all MCUSR registers immediately for 'next use'
  MCUSR = 0;

  // Setup serial, wait for it to be ready so that our logging messages can be
  // read. Note that the baud rate is meaningful on boards that do true serial,
  // while those microcontrollers with builtin USB likely don't rate limit
  // because there isn't a need.
  Serial.begin(57600);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output.
  while (!Serial) {
  }

  // Initialize networking and the server.
  astro_makers::setup();

  astro_makers::logMCUStatusRegister(mcusr);
  astro_makers::logMCUStatusRegister(g_mcusr);
}

void loop() { astro_makers::loop(); }
