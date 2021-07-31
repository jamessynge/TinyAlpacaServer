#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_PWM_LED_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_PWM_LED_H_

// Controls the brightness of one LED, currently using analogWrite with 8-bit
// brightness values.
//
// Author: james.synge@gmail.com

#include "experimental/users/jamessynge/arduino/hostuino/extras/host/arduino/arduino.h"

namespace astro_makers {

class PwmLed {
 public:
  PwmLed(uint8_t output_pin, uint8_t enabled_pin);

  bool is_enabled() const;
  void set_brightness(uint16_t brightness);
  constexpr uint16_t max_brightness() const { return 255; }

 private:
  const uint8_t output_pin_;
  const uint8_t enabled_pin_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_PWM_LED_H_
