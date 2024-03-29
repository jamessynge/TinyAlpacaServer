#ifndef TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_SRC_SERVER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_SRC_SERVER_H_

// Defines setup and loop methods to be called from the setup and loop methods
// of the sketch. This allows the sketch to be simple, focused on just enabling
// the serial device, and may make repurposing this code somewhat easier (TBD if
// it is easy enough).
//
// Author: james.synge@gmail.com

#include <Arduino.h>

namespace fake_weather_service {

void setup();
void loop();

}  // namespace fake_weather_service

#endif  // TINY_ALPACA_SERVER_EXAMPLES_TINYALPACASERVERDEMO_SRC_SERVER_H_
