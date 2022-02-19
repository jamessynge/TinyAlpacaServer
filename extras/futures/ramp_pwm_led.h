#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_RAMP_PWM_LED_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_RAMP_PWM_LED_H_

// Controls the brightness of one LED, with a simple linear fade in/fade out
// used to go from one level to another.
//
// Author: james.synge@gmail.com

#include <Arduino.h>
#include <McuCore.h>

#include "extras/futures/pwm_led.h"

namespace astro_makers {

class RampPwmLed {
 public:
  RampPwmLed(uint8_t output_pin, uint8_t enabled_pin,
             float total_ramp_time_millis);

  bool is_enabled() const;
  void set_brightness_target(uint16_t brightness);
  void set_brightness_immediately(uint16_t brightness);
  uint16_t max_brightness() const { return led_.max_brightness(); }
  uint16_t current_brightness() const { return current_brightness_; }

  bool has_reached_target() const;

  // Call periodically to adjust the brightness towards the target. Uses
  // millis() to compute how much time has elapsed, and therefore how much of an
  // adjustment to make.
  void MaintainDevice();

 private:
  PwmLed led_;
  uint16_t current_brightness_{0};
  uint16_t brightness_target_{0};
  MillisT last_step_time_ms_;
  // NOTE: Could adopt Bresenham's Line Algorithm or similar for tracking the
  // excess, or could just scale the values up by 256 or 65536 here, then scale
  // down when calling led_.set_brightness().
  float excess_{0};
  const float steps_per_ms_;
};

}  // namespace astro_makers

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_RAMP_PWM_LED_H_
