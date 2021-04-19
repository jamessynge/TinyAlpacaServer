#include "src/pwm_led.h"

namespace astro_makers {

PwmLed::PwmLed(uint8_t output_pin, uint8_t enabled_pin)
    : output_pin_(output_pin), enabled_pin_(enabled_pin) {
  pinMode(enabled_pin, INPUT_PULLUP);
  pinMode(output_pin, OUTPUT);
}

bool PwmLed::IsEnabled() { return digitalRead(enabled_pin_) != LOW; }

void PwmLed::setBrightness(uint8_t brightness) {
  if (brightness == 0) {
    digitalWrite(output_pin_, LOW);
  } else {
    analogWrite(output_pin_, brightness);
  }
}

}  // namespace astro_makers
