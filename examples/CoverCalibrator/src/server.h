#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_SERVER_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_SERVER_H_

// Defines setup and loop methods to be called from the setup and loop methods
// of the sketch. This allows the sketch to be simple, focused on just enabling
// the serial device, and may make repurposing this code somewhat easier (TBD if
// it is easy enough).
//
// Author: james.synge@gmail.com

namespace astro_makers {

void setup();
void loop();

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_SERVER_H_
