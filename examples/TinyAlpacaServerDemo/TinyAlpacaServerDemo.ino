// This Arduino Sketch demonstrates how to use Tiny Alpaca Server to respond to
// ASCOM Alpaca requests.
//
// TODO(jamessynge): Add more details about how this demo works.
//
// * On first execution mcunet::IpDevice will generate a random MAC address and
//   a default link-local IP address, and store those in EEPROM for later use.
//
// And on each run:
//
// * Uses DHCP to get an IP address, and falls back to using the IP address
//   stored in EEPROM if necessary.
//
// * If assigned an address via DHCP, keeps the DHCP lease alive over time.
//
// * Handles Alpaca Discovery Protocol messages.
//
// * Provides an HTTP server at the specified port (80 in this demo) that
//   decodes ASCOM Alpaca (HTTP) management and device API requests; the server
//   detects malformed and unsupported requests, for which it returns errors.
//
// * Handles /management/* requests using statically provided information about
//   the server and the devices.
//
// * Handles /setup requests with a simple fixed HTML response.
//   TODO(jamessynge): Improve on this.
//
// * Delegates to registered handlers for valid requests of the form:
//
//       /api/v1/{device_type}/{device_number}/{method}
//       /setup/v1/{device_type}
//
// * Provides methods for handlers for sending OK and error responses, including
//   measuring the size of the JSON body without having to allocate memory for
//   it.
//
// Author: james.synge@gmail.com

#include <Arduino.h>

#include "src/server.h"

void setup() {
  // Setup serial with the fastest baud rate supported by the SoftwareSerial
  // class. Note that the baud rate is meaningful on boards with
  // microcontrollers that do 'true' serial (e.g. Arduino Uno and Mega), while
  // those boards with microcontrollers that have builtin USB (e.g. Arduino
  // Micro) likely don't rate limit because there isn't a need.
  Serial.begin(115200);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output. Note though that a software reset
  // such as that may not reset all of the hardware features, leading to hard
  // to diagnose bugs (experience speaking).
  while (!Serial) {
  }

  // Initialize networking and the server.
  fake_weather_service::setup();
}

void loop() { fake_weather_service::loop(); }
