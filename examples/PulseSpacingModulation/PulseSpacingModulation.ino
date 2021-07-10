#include <Arduino.h>
#include <TinyAlpacaServer.h>

// An experiment in generating a pulse of ~constant duration spaced at varying
// intervals using AVR Timer/Counter interrupts. This is sort of like using the
// PWM feature to generate pulses of varying width, but where the pulse is of
// fixed size and the gap between pulses is not.
//
// Why? To generate stepper motor pulses at a rate that I can control better
// than if doing so from inside the loop() function when there is also other
// work to do during loop which may take more time that the desired gap between
// pulses.
//
// Concretely, Alan has used AccelStepper to drive the CoverCalibrator stepper
// motor at a requesed 10K microsteps per second (a step every 100us), which
// seemed to be the maximum achieveable speed (above that the motor would miss
// steps or completely stall).
//
// I've learned how to write this from various sources: primarily the Microchip
// datasheet for the ATmega2560, but also from various public examples,
// including TimerOne which has a good example of how to use the phase correct
// PWM mode and the interrupt on overflow feature of the AVR chip.
//
// I also found this page useful for learning and understanding the terminology
// of stepper motors and the torque they can generate:
//
//   https://www.orientalmotor.com/stepper-motors/technology/speed-torque-curves-for-stepper-motors.html

// These are the rev 6 pin definitions, not necessarily those we desire to use
// long-term:
#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 4
#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21

#define kMaximumAllowedSteps 150000

namespace {
enum MovementMode {
  kNotMoving,
  kStopMoving,
  kOpening,
  kClosing,
};

volatile MovementMode movement_mode = kNotMoving;

enum class ClockPrescaling : uint8_t {
  kDivideBy1 = 0 << CS52 | 0 << CS52 | 1 << CS50,
  kDivideBy8 = 0 << CS52 | 1 << CS52 | 0 << CS50,
  kDivideBy64 = 0 << CS52 | 1 << CS52 | 1 << CS50,
  kDivideBy256 = 1 << CS52 | 0 << CS52 | 0 << CS50,
  kDivideBy1024 = 1 << CS52 | 0 << CS52 | 1 << CS50,
};

// volatile uint8_t *step_pin_port;
// volatile uint8_t step_pin_mask;

volatile uint8_t limit_pin;
// volatile uint8_t* limit_pin_port;
// volatile uint8_t limit_pin_mask;

// If > 0, then is reduced until it reaches zero at which point the counter is
// disabled.
volatile bool apply_allowed_steps = false;
volatile uint32_t allowed_steps = 0;
volatile uint32_t step_count = 0;

volatile uint16_t target_top = 0;

// Amount of time we spent
volatile uint32_t start_timer_micros = 0;
volatile uint32_t stop_timer_micros = 0;

void DisableTimer5() {
  TCCR5A = 0;
  TCCR5B = 0;
  TCCR5C = 0;
  TIMSK5 = 0;
  ICR5 = 0;
  TCNT5 = 0;
}

// F_CPU is defined by the build system to the (standard) number of system clock
// cycles per second (e.g. 16,000,000 for the ATmega2560). Note that we depend
// here on the compiler performing these calculations at compile time and just
// leaving us with constants in the expressions.
constexpr double kSystemClockTicksPerMicrosecond = F_CPU / 1000000.0;
constexpr double kSystemClockTickDurationMicroseconds =
    1.0 / kSystemClockTicksPerMicrosecond;
constexpr uint32_t kMaxDualSlopeTicks = 0xFFFF * 2UL;
constexpr uint32_t kPrescaleBy1MaxDualSlopeMicroseconds = static_cast<uint32_t>(
    kMaxDualSlopeTicks * kSystemClockTickDurationMicroseconds + 0.5);
constexpr uint32_t kPrescaleBy8MaxDualSlopeMicroseconds = static_cast<uint32_t>(
    kMaxDualSlopeTicks * kSystemClockTickDurationMicroseconds * 8 + 0.5);
constexpr uint32_t kPrescaleBy64MaxDualSlopeMicroseconds =
    static_cast<uint32_t>(
        kMaxDualSlopeTicks * kSystemClockTickDurationMicroseconds * 64 + 0.5);
constexpr uint32_t kPrescaleBy256MaxDualSlopeMicroseconds =
    static_cast<uint32_t>(
        kMaxDualSlopeTicks * kSystemClockTickDurationMicroseconds * 256 + 0.5);
constexpr uint32_t kPrescaleBy1024MaxDualSlopeMicroseconds =
    static_cast<uint32_t>(
        kMaxDualSlopeTicks * kSystemClockTickDurationMicroseconds * 1024 + 0.5);

// period_us is the desired time between interrupts in microseconds. We use WGM
// mode 9, i.e. Phase and Frequency Correct PWM Mode. In this mode TCNT5 is
// incremented by one at each clock tick until it reaches TOP (OCR5A) , then
// decremented down to BOTTOM (0), at which point it starts incrementing up
// again. This dual slope operation means that the period is 2 * TOP, i.e. 2 *
// OCR5A. Note that the counter is at TOP for only one clock cycle (I don't know
// if that also means that the counter is at BOTTOM for only one clock cycle,
// but that seems likely).
//
// To achieve an interrupt interval closest to the requested period, we want to
// use the least amount of prescaling of the system clock such that twice the
// value of MAX (0xFFFF) is bigger than the desired period; this results in the
// smallest time between one counter increment and the next.
//
// The overflow interrupt occurs each time the counter reaches BOTTOM.
void AdjustTimerPeriod(uint32_t period_ns) {
  uint8_t a = 1 << WGM50;
  uint8_t b = 1 << WGM53;
  uint16_t top;

  // Convert the period into 1/2 the number of system clock cycles for that same
  // duration. This somewhat convoluted expression deals with the precision
  // issues of multiplying and dividing integers. For example (F_CPU / 100,000)
  // is the number of clock cycles in 10 microseconds.
  const uint32_t half_period_cycles = ((F_CPU / 100000) * period_ns) / 20000;
  constexpr uint32_t kMaxCount = 65536UL;

  if (half_period_cycles < kMaxCount) {
    b |= static_cast<uint8_t>(ClockPrescaling::kDivideBy1);
    top = half_period_cycles;
  } else if (half_period_cycles < kMaxCount * 8) {
    b |= static_cast<uint8_t>(ClockPrescaling::kDivideBy8);
    top = half_period_cycles / 8;
  } else if (half_period_cycles < kMaxCount * 64) {
    b |= static_cast<uint8_t>(ClockPrescaling::kDivideBy64);
    top = half_period_cycles / 64;
  } else if (half_period_cycles < kMaxCount * 256) {
    b |= static_cast<uint8_t>(ClockPrescaling::kDivideBy256);
    top = half_period_cycles / 256;
  } else {
    TAS_DCHECK_LT(half_period_cycles, kMaxCount * 1024);
    b |= static_cast<uint8_t>(ClockPrescaling::kDivideBy1024);
    top = half_period_cycles / 1024;
  }

  target_top = top;
  auto initial_top = top * 32UL;
  top = min(65535UL, initial_top);

  TAS_VLOG(1) << TAS_FLASHSTR("period_ns=") << period_ns
              << TAS_FLASHSTR(", target_top=") << target_top
              << TAS_FLASHSTR(", top=") << top << TAS_FLASHSTR(" (")
              << alpaca::BaseHex << top << TAS_FLASHSTR("), a=") << a
              << TAS_FLASHSTR(", b=") << b;
  delay(20);

  noInterrupts();
  OCR5A = top;
  TCCR5B = b;
  TCCR5A = a;
  TCNT5 = 1;
  interrupts();
}

void StartTimer5(uint32_t period_ns) {
  DisableTimer5();

  TAS_VLOG(5) << TAS_FLASHSTR("StartTimer5(") << period_ns << ')';
  delay(20);

  AdjustTimerPeriod(period_ns);
  start_timer_micros = micros();
  bitWrite(TIMSK5, TOIE5, 1);
}

void StartMoving(MovementMode new_movement_mode, uint8_t limit_switch_pin,
                 double steps_per_second) {
  MovementMode copy = movement_mode;
  if (copy != kNotMoving) {
    // Disable timer 5
    DisableTimer5();
    movement_mode = kNotMoving;
    TAS_VLOG(1) << TAS_FLASHSTR("movement_mode was ") << copy
                << TAS_FLASHSTR(", now ") << movement_mode;
  }

  TAS_VLOG(1) << TAS_FLASHSTR("StartMoving(") << new_movement_mode
              << TAS_FLASHSTR(", ") << limit_switch_pin << TAS_FLASHSTR(", ")
              << steps_per_second << ')';
  delay(20);

  limit_pin = limit_switch_pin;

  if (digitalRead(limit_pin) == LOW) {
    TAS_VLOG(1) << TAS_FLASHSTR("Limit switch is closed, so no need to move.");
    return;
  }

  // limit_pin_port = portOutputRegister(digitalPinToPort(limit_pin));
  // limit_pin_mask = digitalPinToBitMask(limit_pin);

  // double clock_ticks_per_second = 16000000.0;
  // uint16_t clock_ticks_per_step = clock_ticks_per_second / steps_per_second;

  apply_allowed_steps = true;
  allowed_steps = kMaximumAllowedSteps;
  step_count = 0;

  movement_mode = new_movement_mode;

  uint32_t period_ns = 1000000000.0 / steps_per_second + 0.5;

  StartTimer5(period_ns);
}

void DoOpen(uint16_t steps_per_second) {
  // Set the direction.
  digitalWrite(kCoverMotorDirectionPin, LOW);

  // Start moving the cover in that direction.
  StartMoving(kOpening, kCoverOpenLimitPin, steps_per_second);
}

void DoClose(uint16_t steps_per_second) {
  // Set the direction.
  digitalWrite(kCoverMotorDirectionPin, HIGH);

  // Start moving the cover in that direction.
  StartMoving(kClosing, kCoverCloseLimitPin, steps_per_second);
}

}  // namespace

ISR(TIMER5_OVF_vect) {
  if (movement_mode == kOpening || movement_mode == kClosing) {
    // Reached the limit?
    if (digitalRead(limit_pin) == HIGH) {
      // No, so step now.
      // Writing using step_pin_port and step_pin_mask is likely to be much
      // faster.
      digitalWrite(kCoverMotorStepPin, HIGH);
      delayMicroseconds(2);
      digitalWrite(kCoverMotorStepPin, LOW);
      ++step_count;
      if (!apply_allowed_steps || allowed_steps > step_count) {
        if (target_top != 0) {
          uint16_t current_target = OCR5A;
          uint32_t new_target = (current_target * 15 + target_top) / 16;
          OCR5A = new_target;
          if (new_target <= target_top) {
            target_top = 0;
          }
        }
        return;
      }
    }
  }

  // Shutoff the timer...
  stop_timer_micros = micros();
  movement_mode = kNotMoving;
  TIMSK5 = 0;
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

  pinMode(kCoverMotorStepPin, OUTPUT);
  pinMode(kCoverMotorDirectionPin, OUTPUT);
  pinMode(kCoverOpenLimitPin, INPUT_PULLUP);
  pinMode(kCoverCloseLimitPin, INPUT_PULLUP);
}

void loop() {
  static bool open_next = true;
  static uint16_t steps_per_second = 19500;
  static uint16_t start_count = 0;

  if (movement_mode == kNotMoving) {
    if (step_count > 0) {
      const auto elapsed_us = stop_timer_micros - start_timer_micros;
      const double elapsed_s = elapsed_us / 1000000.0;
      const double steps_per_s = step_count / elapsed_s;
      const double pct_target = steps_per_s / steps_per_second * 100.0;
      TAS_VLOG(1) << TAS_FLASHSTR("steps=") << step_count
                  << TAS_FLASHSTR(", elapsed_us=") << elapsed_us
                  << TAS_FLASHSTR(", elapsed_s=") << elapsed_s
                  << TAS_FLASHSTR(", steps/s=") << steps_per_s
                  << TAS_FLASHSTR(", % target steps/s=") << pct_target << '\n';
    }
    start_timer_micros = 0;
    stop_timer_micros = 0;
    step_count = 0;
    if (++start_count > 4) {
      start_count = 0;
      steps_per_second += 50;
    }
    if (open_next) {
      open_next = false;
      Serial.println("DoOpen ...");
      // Longer because in the current orientation of the prototype on a table
      // top, open requires more torque at the start because the cover is
      // horizontal, so the motor has the most work to do to move it from
      // stationary to rotating "upwards".
      delay(500);
      DoOpen(steps_per_second);
    } else {
      open_next = true;
      Serial.println("DoClose ...");
      delay(100);
      DoClose(steps_per_second);
    }
  }
}
