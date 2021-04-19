#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_PWM_LED_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_PWM_LED_H_

// Controls the brightness of one LED, currently using analogWrite with 8-bit
// brightness values.

#include <Arduino.h>

namespace astro_makers {

class PwmLed {
 public:
  PwmLed(uint8_t output_pin, uint8_t enabled_pin);

  bool IsEnabled();

  void setBrightness(uint8_t brightness);

 private:
  const uint8_t output_pin_;
  const uint8_t enabled_pin_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_PWM_LED_H_
