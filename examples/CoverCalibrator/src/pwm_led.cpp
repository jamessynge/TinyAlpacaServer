#include "pwm_led.h"

namespace astro_makers {

PwmLed::PwmLed(uint8_t output_pin, uint8_t enabled_pin)
    : output_pin_(output_pin), enabled_pin_(enabled_pin) {
  pinMode(enabled_pin, INPUT_PULLUP);
  pinMode(output_pin, OUTPUT);
}

bool PwmLed::is_enabled() const { return digitalRead(enabled_pin_) != LOW; }

void PwmLed::set_brightness(uint16_t brightness) {
  if (brightness == 0) {
    digitalWrite(output_pin_, LOW);
  } else {
    analogWrite(output_pin_, brightness);
  }
}

}  // namespace astro_makers
