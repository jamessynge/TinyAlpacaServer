// A modification of Alan's demo of the cover-calibrator hardware to deal with
// my remapping of the pins.

#include <Arduino.h>
#include <TinyAlpacaServer.h>

// Modified pin selection to avoid pins used for other purposes.
// Pins we should (or must) avoid on the Robotdyn Mega ETH:
// D00 - RX (Serial over USB)
// D01 - TX (Serial over USB)
// D04 - SDcard Chip Select
// D10 - W5500 Chip Select
// D13 - Built-in LED
// D50 - MISO (SPI)
// D51 - MOSI (SPI)
// D52 - SCK (SPI)

#define kLedChannel1PwmPin 5           // OC3A      unchanged
#define kLedChannel2PwmPin 6           // OC4A      unchanged
#define kLedChannel2EnabledPin PIN_A1  //           was 10
#define kLedChannel3PwmPin 7           // OC4B      unchanged
#define kLedChannel3EnabledPin PIN_A2  //           was 11
#define kLedChannel4PwmPin 8           // OC4C      unchanged
#define kLedChannel4EnabledPin PIN_A3  //           was 12
#define kCoverMotorStepPin 3           //           unchanged
#define kCoverMotorDirectionPin 5      //           unchanged
#define kCoverOpenLimitPin 20          //           unchanged
#define kCoverCloseLimitPin 21         //           unchanged
#define kCoverEnabledPin PIN_A4        //           was 13

using ::alpaca::TimerCounter3Pwm16Output;
using ::alpaca::TimerCounter4Pwm16Output;
using ::alpaca::TimerCounterChannel;

TimerCounter3Pwm16Output led1(TimerCounterChannel::A);
TimerCounter4Pwm16Output led2(
    TimerCounterChannel::A /*, kLedChannel2EnabledPin */);
TimerCounter4Pwm16Output led3(
    TimerCounterChannel::B /*, kLedChannel3EnabledPin */);
TimerCounter4Pwm16Output led4(
    TimerCounterChannel::C /*, kLedChannel4EnabledPin */);

#define DUMP_TCCR(n)               \
  Serial.print("TCCR" #n "A: 0b"); \
  Serial.println(TCCR##n##A, 2);   \
  Serial.print("TCCR" #n "B: 0b"); \
  Serial.println(TCCR##n##B, 2);   \
  Serial.print("TCCR" #n "C: 0b"); \
  Serial.println(TCCR##n##C, 2);   \
  Serial.println()

#define DUMP_OCR(n)             \
  Serial.print("OCR" #n "A: "); \
  Serial.println(OCR##n##A);    \
  Serial.print("OCR" #n "B: "); \
  Serial.println(OCR##n##B);    \
  Serial.print("OCR" #n "C: "); \
  Serial.println(OCR##n##C);    \
  Serial.println()

#define DUMP_TC(n)             \
  DUMP_TCCR(n);                \
  Serial.print("ICR" #n ": "); \
  Serial.println(ICR##n);      \
  DUMP_OCR(n);                 \
  Serial.println()

#define DUMP_USED_TIMER_COUNTERS \
  DUMP_TC(3);                    \
  DUMP_TC(4)

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read. Note that the baud rate is meaningful on boards that do true serial,
  // while those microcontrollers with builtin USB likely don't rate limit
  // because there isn't a need.
  Serial.begin(115200);

  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost. Note that this isn't true for all
  // Arduino-like boards: some reset when the Serial Monitor connects, so we
  // almost always get the initial output.
  while (!Serial) {
  }

  DUMP_USED_TIMER_COUNTERS;

  TAS_VLOG(1) << TAS_FLASHSTR("Initializing 16-bit PWM");

  pinMode(kLedChannel1PwmPin, OUTPUT);
  pinMode(kLedChannel2PwmPin, OUTPUT);
  pinMode(kLedChannel3PwmPin, OUTPUT);
  pinMode(kLedChannel4PwmPin, OUTPUT);

  TimerCounter3Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);
  TimerCounter4Initialize16BitFastPwm(alpaca::ClockPrescaling::kDivideBy1);

  TAS_VLOG(1) << TAS_FLASHSTR("Initialized 16-bit PWM");
  Serial.println();

  DUMP_USED_TIMER_COUNTERS;
}

template <class T>
void Sweep16BitPwm(T& t, uint16_t increment, MillisT delay_by,
                   const char* name) {
  if (t.IsEnabled()) {
    TAS_VLOG(1) << name << TAS_FLASHSTR(" is enabled.");
    TAS_VLOG(1) << TAS_FLASHSTR("Sweeping ") << name
                << TAS_FLASHSTR(" upwards");

    int32_t value = 1;
    while (value <= t.max_count()) {
      t.set_pulse_count(static_cast<uint16_t>(value & 0xFFFF));

      if (value == 1 || (value + increment) >= t.max_count()) {
        TAS_VLOG(1) << TAS_FLASHSTR("value = ") << value;
        DUMP_USED_TIMER_COUNTERS;
      }

      value += increment;
      delay(delay_by);
    }
    TAS_VLOG(1) << TAS_FLASHSTR("Sweeping ") << name
                << TAS_FLASHSTR(" downwards");
    value = t.max_count();
    while (value >= 1) {
      t.set_pulse_count(static_cast<uint16_t>(value & 0xFFFF));
      value -= increment;
      delay(delay_by);
    }
    TAS_VLOG(1) << TAS_FLASHSTR("value = ") << value;

    DUMP_USED_TIMER_COUNTERS;

    TAS_VLOG(1) << TAS_FLASHSTR("Turning ") << name << TAS_FLASHSTR(" off");
    t.set_pulse_count(0);

    DUMP_USED_TIMER_COUNTERS;

  } else {
    TAS_VLOG(1) << name << TAS_FLASHSTR(" is disabled.");
  }
}

void loop() {
  Sweep16BitPwm(led1, 50, 4, "led1");
  //  delay(120 * 1000U);
  Sweep16BitPwm(led2, 50, 4, "led2");
  //  delay(120 * 1000U);
  Sweep16BitPwm(led3, 50, 4, "led3");
  //  delay(120 * 1000U);
  Sweep16BitPwm(led4, 50, 4, "led4");
  //  delay(120 * 1000U);
}
