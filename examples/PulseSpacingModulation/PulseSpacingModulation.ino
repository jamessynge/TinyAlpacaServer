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
// seemed to be the maximum achieveable

#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 5
#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21

#define kStepsPerSecond 20000

enum MovementMode {
  kNotMoving,
  kStopMoving,
  kOpening,
  kClosing,
};

volatile MovementMode movement_mode = kNotMoving;

// volatile uint8_t *step_pin_port;
// volatile uint8_t step_pin_mask;

volatile uint8_t limit_pin;
volatile uint8_t* limit_pin_port;
volatile uint8_t limit_pin_mask;

// If > 0, then is reduced until it reaches zero at which point the counter is
// disabled.
volatile bool apply_allowed_steps = false;
volatile uint32_t allowed_steps = 0;
volatile uint32_t step_count = 0;

inline void DoStep() {
  // Writing using step_pin_port and step_pin_mask is likely to be much faster.
  digitalWrite(kCoverMotorStepPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(kCoverMotorStepPin, LOW);
}

ISR(TIMER5_COMPC_vect) {
  if (movement_mode == kOpening || movement_mode == kClosing) {
    // Reached the limit?
    if (digitalRead(limit_pin) == HIGH) {
      // No, so step now.
      // Writing using step_pin_port and step_pin_mask is likely to be much
      // faster.
      digitalWrite(kCoverMotorStepPin, HIGH);
      delayMicroseconds(1);
      digitalWrite(kCoverMotorStepPin, LOW);
      ++step_count;
      if (!apply_allowed_steps || allowed_steps > ++step_count) {
        return;
      }
    }
  }

  // Shutoff the timer...

  // Disable timer 5
  bitWrite(TIMSK5, OCIE5C, 0);
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

  // Put timer 5 into "Clear Timer on Compare Match" (CTC) Mode, with no scaling
  // of the built-in clock (i.e. 16MHz if a normal Arduino Mega).
  TCCR5A = 0;
  TCCR5B = 1 << WGM52 | 1 << CS50;
}

void StartMoving(MovementMode new_movement_mode, uint8_t limit_switch_pin,
                 double steps_per_second) {
  if (movement_mode != kNotMoving) {
    // Disable timer 5
    bitWrite(TIMSK5, OCIE5C, 0);
    movement_mode = kNotMoving;
  }

  limit_pin = limit_switch_pin;

  if (digitalRead(limit_pin) == LOW) {
    // Limit switch is closed, so no need to move.
    return;
  }

  // limit_pin_port = portOutputRegister(digitalPinToPort(limit_pin));
  // limit_pin_mask = digitalPinToBitMask(limit_pin);

  double clock_ticks_per_second = 16000000.0;
  uint16_t clock_ticks_per_step = clock_ticks_per_second / steps_per_second;

  apply_allowed_steps = true;
  allowed_steps = 120000;
  step_count = 0;

  movement_mode = new_movement_mode;
  OCR5C = clock_ticks_per_step;
  TCNT5 = 0;
  bitWrite(TIMSK5, OCIE5A, 1);
}

void DoOpen() {
  // Set the direction.
  digitalWrite(kCoverMotorDirectionPin, LOW);

  // Start moving the cover in that direction.
  StartMoving(kOpening, kCoverOpenLimitPin, kStepsPerSecond);
}

void DoClose() {
  // Set the direction.
  digitalWrite(kCoverMotorDirectionPin, HIGH);

  // Start moving the cover in that direction.
  StartMoving(kClosing, kCoverCloseLimitPin, kStepsPerSecond);
}

void loop() {
  static bool open_next = true;
  static uint32_t start_micros = 0;
  auto now = micros();

  if (movement_mode != kNotMoving) {
    return;
  }

  if (step_count > 0) {
    auto elapsed = now - start_micros;
    Serial.print("step_count=");
    Serial.print(step_count);
    Serial.print(", elapsed_micros=");
    Serial.print(elapsed);
    Serial.print(", micros/step=");
    Serial.println(elapsed / step_count);
    Serial.println();
  }

  if (open_next) {
    open_next = false;
    Serial.println("DoOpen ...");
    delay(100);
    start_micros = micros();
    DoOpen();
  } else {
    open_next = true;
    Serial.println("DoClose ...");
    delay(100);
    start_micros = micros();
    DoClose();
  }
}
