#include "extras/futures/ramp_pwm_led.h"

#include "logging.h"

namespace astro_makers {

RampPwmLed::RampPwmLed(uint8_t output_pin, uint8_t enabled_pin,
                       float total_ramp_time_millis)
    : led_(output_pin, enabled_pin),
      steps_per_ms_(led_.max_brightness() / total_ramp_time_millis) {}

bool RampPwmLed::is_enabled() const { return led_.is_enabled(); }

void RampPwmLed::set_brightness_target(uint16_t brightness) {
  TAS_DCHECK_LT(brightness, max_brightness());
  if (brightness_target_ == brightness) {
    return;
  }
  brightness_target_ =
      brightness > max_brightness() ? max_brightness() : brightness;
  excess_ = 0;
  last_step_time_ms_ = millis();
}

void RampPwmLed::set_brightness_immediately(uint16_t brightness) {
  TAS_DCHECK_LT(brightness, max_brightness());
  brightness_target_ =
      brightness > max_brightness() ? max_brightness() : brightness;
  if (!has_reached_target()) {
    current_brightness_ = brightness;
    led_.set_brightness(brightness);
  }
}

bool RampPwmLed::has_reached_target() const {
  return brightness_target_ == current_brightness_;
}

void RampPwmLed::MaintainDevice() {
  if (has_reached_target()) {
    return;
  }
  auto now = millis();
  auto elapsed = now - last_step_time_ms_;
  if (elapsed == 0) {
    return;
  }
  // NOTE: Could adopt Bresenham's Line Algorithm or similar for tracking
  // the excess, or could just scale the values up by 256 or 65536 here,
  // then scale down when calling led_.set_brightness(). Either way, we
  // could avoid the use of floats.
  const uint16_t lower_limit = current_brightness_ < brightness_target_
                                   ? current_brightness_
                                   : brightness_target_;
  const uint16_t upper_limit = current_brightness_ < brightness_target_
                                   ? brightness_target_
                                   : current_brightness_;
  const float delta =
      current_brightness_ < brightness_target_ ? steps_per_ms_ : -steps_per_ms_;
  float ideal_brightness = current_brightness_ + excess_ + elapsed * delta;

  if (ideal_brightness > upper_limit) {
    ideal_brightness = upper_limit;
  } else if (ideal_brightness <= lower_limit) {
    ideal_brightness = lower_limit;
  }
  uint16_t new_brightness = static_cast<uint16_t>(ideal_brightness + 0.5f);
  excess_ = ideal_brightness - new_brightness;
  led_.set_brightness(current_brightness_ = new_brightness);
}

}  // namespace astro_makers
