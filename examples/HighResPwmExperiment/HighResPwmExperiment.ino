#include <Arduino.h>

// Using D13 as a PWM output because it already has an LED on it.

enum class TestMode {
  LedOff,
  LedOn,
  LedBlink,
  LoResRamp,
  HiResRamp
};

static constexpr TestMode kTestMode = TestMode::LoResRamp;

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
  // TOP, with system clock used without prescaling. All 3 output pins are set to
  // be non-inverting: the output for channel A will be set when the counter
  // starts at BOTTOM, then will be clearned when the counter reaches the value of
  // OCR1A. OCR1B is the corresponding match register for channel B, and OCR1C is
  // for channel C.
  //
  // With these settings and a 16MHz clock, the T/C has a 244.14 cycles per
  // second, so will be invisible to humans, but a very short exposure (e.g.
  // video) risks seeing N or N + 1 flashes of the LED, for a value of N
  // under 10. If the brightness of the LEDs is low enough at a duty cycle of 1
  // in 5000, for example, we could raise N to 53 for an exposure time of 1/60th
  // of a second.

  noInterrupts();
  TCCR1A = 1 << WGM11 | 1 << COM1A1 | 1 << COM1B1 | 1 << COM1C1;
  TCCR1B = 1 << CS10 | 1 << WGM12 | 1 << WGM13;
  ICR1 = UINT16_MAX;
  interrupts();
}

static int32_t brightness = 0;
static int32_t brightness_delta = 0;
static int32_t max_brightness;
static unsigned long loop_delay;

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

  Serial.print("TCCR1A: 0b");
  Serial.println(TCCR1A, 2);
  Serial.print("TCCR1B: 0b");
  Serial.println(TCCR1B, 2);
  Serial.print("TCCR1C: 0b");
  Serial.println(TCCR1C, 2);

  Serial.println();
  Serial.print("Blinking LEDs to signal that we're running...");

  pinMode(13, OUTPUT);

  for (int i = 0; i <= 20; ++i) {
    digitalWrite(13, (i & 1) ? HIGH : LOW);
    delay(50);
  }

  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);

  Serial.println(" done.");

  switch (kTestMode) {
    case TestMode::LedOff:
      loop_delay = 1000;
      break;

    case TestMode::LedOn:
      loop_delay = 1000;
      break;

    case TestMode::LedBlink:
      loop_delay = 500;
      brightness_delta = 1;
      max_brightness = 1;
      break;

    case TestMode::LoResRamp:
      // Delay for 250ms, yielding ~4 loops per second, so a cycle time of 64
      // seconds to go from 0 to 255, and the same time to go back down to zero.
      loop_delay = 250;
      brightness_delta = 1;
      max_brightness = 0xffL;

      loop_delay = 50;  
      break;

    case TestMode::HiResRamp:
      OCR1C = brightness;
      timer1_enable_hi_res_pwm();

      // Delay for 1, yielding ~1000 loops per second, so a cycle time of 65.536
      // seconds to go from 0 to 255, and the same time to go back down to zero.
      loop_delay = 1;
      brightness_delta = 1;
      max_brightness = 0xffffL;
      break;
  }

  Serial.println();
  Serial.println("After TestMode based initialization:");

  Serial.println();
  Serial.print("max_brightness: ");
  Serial.println(max_brightness);
  Serial.print("brightness: ");
  Serial.println(brightness);
  Serial.print("brightness_delta: ");
  Serial.println(brightness_delta);
  Serial.print("loop_delay: ");
  Serial.println(loop_delay);
  Serial.print("ramp duration: ");
  Serial.print((max_brightness + 1.0) / (1000.0 / loop_delay));
  Serial.println(" seconds");

  Serial.println();
  Serial.print("TCCR1A: 0b");
  Serial.println(TCCR1A, 2);
  Serial.print("TCCR1B: 0b");
  Serial.println(TCCR1B, 2);
  Serial.print("TCCR1C: 0b");
  Serial.println(TCCR1C, 2);

  Serial.println();
  Serial.println("End of setup");

  
}

void loop() {

  if (brightness_delta != 0 && (brightness <= 0 || brightness >= max_brightness)) {
      Serial.println();
      Serial.println("loop start");
      Serial.print("brightness: ");
      Serial.println(brightness);
      Serial.print("brightness_delta: ");
      Serial.println(brightness_delta);
  }

  switch (kTestMode) {
    case TestMode::LedOff:
      digitalWrite(13, LOW);
      break;

    case TestMode::LedOn:
      digitalWrite(13, HIGH);
      break;

    case TestMode::LedBlink:
      digitalWrite(13, (brightness > 0) ? HIGH : LOW);
      break;

    case TestMode::LoResRamp:
      analogWrite(13, brightness & 0xff);
      break;

    case TestMode::HiResRamp:
      OCR1C = brightness & 0xffff;
      break;
  }

  if (brightness_delta != 0) {
    brightness += brightness_delta;
    bool flipped = false;
    if (brightness < 0) {
      brightness = -brightness;
      brightness_delta = -brightness_delta;
      flipped = true;
    } else if (brightness > max_brightness) {
      brightness = max_brightness - (brightness - max_brightness);
      brightness_delta = -brightness_delta;
      flipped = true;
    }
    if (flipped) {
      Serial.println();
      Serial.println("flipped direction");
      Serial.print("brightness: ");
      Serial.println(brightness);
      Serial.print("brightness_delta: ");
      Serial.println(brightness_delta);
    }
  }

  delay(loop_delay);


}
