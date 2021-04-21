#include <Arduino.h>

static constexpr bool kDoHighRes = false;

// Experiments with High Resolution (16-bit) PWM.
//
// General notes, especially terminology:
//
// Timer/Counter (T/C) Control Register (TCCRn) is split into two 8-bit
// registers, TCCRnA and TCCRnB. The 8-bit and 16-bit t/c have different bit
// layouts, and timer 2 (8-bit t/c with async operation) is somewhat different
// from timer 1 (8-bit t/c), and has different prescaler options, IIUC.
//
// WGMn: Waveform Generation Mode for T/C number n
//
// WGMnm: Bit m of WGMn (e.g. bits 3, 2, 1 and 0, allowing for 16 modes)
//
// OCnX: Output Compare (OC) pin for T/C n and channel X.
//
// OCRnX: Output Compare Register for T/C n channel X.
//
// DDR: Data Direction Register.
//
// COMnX: Compare Output Mode for T/C n and Output Compare pin nX.
//
// COMnXm: Bit m of COMnX (e.g. bits 1 and 0). If either bit is 1, the OCnX
// output is driven by the T/C n behavior. Requires that the DDR configure the
// pin for output.
//
// CSn: Clock Select for T/C n. Configures the clock source and prescaling.
// For example 0 disables the T/C, 0b010 divides the clock frequency by 8 (e.g
// 2MHz for a 16MHz system clock).
//
// CSnm: Bit m of CSn (e.g. bits 2, 1, and 0 of the Clock Select value).
// Fast PWM: single slope counter, when it reaches TOP it clears the counter to
// BOTTOM, rather than counting down to BOTTOM again.
//
// Phase Correct PWM: dual slope counter, counts up from BOTTOM to TOP, then
// counts down to BOTTOM again. Lather, rinse, repeat.

void timer1_enable_hi_res_pwm() {
  // Put Timer/Counter #1 into mode 14: Fast PWM, uses value of ICRn (0xFFFF) as
  // TOP, with system clock used without prescaling. Both output pins are set to
  // be inverting: the output for channel A will be set high the counter reaches
  // the value of OCR1A, and cleared when it wraps around from TOP to BOTTOM
  // (0). OCR1B is the corresponding match register for channel B, and OCR1C is
  // for channel C.
  //
  // With these settings and a 16MHz clock, the T/C has a 244.14 cycles per
  // second, so will be invisible to humans, but a very short exposure (e.g.
  // video) risks seeing N or N + 1 flashes of the LED, for a value of N
  // under 10. If the brightness of the LEDs is low enough at a duty cycle of 1
  // in 5000, for example, we could raise N to 53 for an exposure time of 1/60th
  // of a second.

  noInterrupts();
  TCCR1A = 1 << WGM11 | 1 << COM1A1 | 1 << COM1B1;
  TCCR1B = 1 << CS10 | 1 << WGM12 | 1 << WGM13;
  ICR1 = UINT16_MAX;
  interrupts();
}

void setup() {
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

  pinMode(13, OUTPUT);

  if (kDoHighRes) {
    timer1_enable_hi_res_pwm();
  }
}

void loop() {
  static uint16_t pwm = 0;
  // Using D13 as a PWM output because it already has an LED on it.

  if (kDoHighRes) {
    // Only the low 8 bits will be used.
    analogWrite(13, pwm++);
    // Delay for 50ms, yielding ~20 loops per second, so a cycle time of 12.8
    // seconds.
    delay(50);
  } else {
    OCR1C = pwm;
    pwm += 5;
    // 65536 / 5 steps, 1 step per millisecond, so a cycle time of 13.1 seconds.
    delay(1);
  }
}
