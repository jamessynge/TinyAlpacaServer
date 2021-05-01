#include <Arduino.h>
#include <TinyAlpacaServer.h>

// This is an experiment with using PWM (or timer/counter interrupts) for
// controlling the stepper motor for the cover of the cover-calibrator, and
// using interrupts for detecting when the open or close limit switches are
// closed, i.e. they are pressed by the (moving) cover.
//
// Why? The AccelStepper library requires either frequent calls (e.g. from
// loop) to move one step at a time, in which case movement is smooth only if
// loop is much faster than the time between steps, or requires a call to
// AccelStepper::runToPosition (or similar), which is a blocking call. If we use
// PWM or an interrupt, there is a good chance we can achieve quite smooth
// movement, assuming that the mainline code doesn't have long sections with
// interrupts blocked.
//
// Author: james.synge@gmail.com

// -----------------------------------------------------------------------------
// From cover_calibrator.cc:
// Pins we should (or must) avoid on the Robotdyn Mega ETH:
// D00 - RX (Serial over USB)
// D01 - TX (Serial over USB)
// D04 - SDcard Chip Select
// D09 - SDcard Detect
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
#define kCoverOpenLimitPin 20          // INT1      unchanged
#define kCoverCloseLimitPin 21         // INT0      unchanged
#define kCoverEnabledPin PIN_A4        //           was 13

namespace {

enum TestMode { kCountBounces, kFirstPress };

constexpr TestMode kTestMode = kCountBounces;

constexpr uint8_t kCoverOpenInterruptNumber =
    digitalPinToInterrupt(kCoverOpenLimitPin);
constexpr uint8_t kCoverCloseInterruptNumber =
    digitalPinToInterrupt(kCoverCloseLimitPin);

class PressInfo {
 public:
  PressInfo(const char* name) { Reset(); }

  void Reset() {
    press_count_ = 0;
    first_press_micros_ = 0;
    latest_press_micros_ = 0;
  }

  void Pressed() {}

 private:
  volatile uint8_t press_count_;
  volatile uint32_t first_press_micros_;
  volatile uint32_t latest_press_micros_;
};

volatile uint8_t open_pressed_count = 0;
volatile uint32_t first_open_pressed_micros = 0;
volatile uint32_t latest_open_pressed_micros = 0;

volatile uint8_t close_pressed_count = 0;
volatile uint32_t first_close_pressed_micros = 0;
volatile uint32_t latest_close_pressed_micros = 0;

void OpenPressed() {
  auto now = micros();
  if (open_pressed_count == 0) {
    first_open_pressed_micros = now;
    if (kTestMode == kFirstPress) {
      detachInterrupt(kCoverOpenInterruptNumber);
    }
  }
  latest_open_pressed_micros = now;
  ++open_pressed_count;
}

void ClosePressed() {
  auto now = micros();
  if (close_pressed_count == 0) {
    first_close_pressed_micros = now;
    if (kTestMode == kFirstPress) {
      detachInterrupt(kCoverCloseInterruptNumber);
    }
  }
  latest_close_pressed_micros = now;
  ++close_pressed_count;
}

}  // namespace

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
  Serial.println("Serial ready");

  pinMode(kCoverOpenLimitPin, INPUT_PULLUP);
  pinMode(kCoverCloseLimitPin, INPUT_PULLUP);

  attachInterrupt(kCoverOpenInterruptNumber, OpenPressed, FALLING);
  attachInterrupt(kCoverCloseInterruptNumber, ClosePressed, FALLING);
}

void report_presses(const char* name, volatile uint8_t& count, uint32)

    volatile uint8_t open_pressed_count = 0;
volatile uint32_t first_open_pressed_micros = 0;
volatile uint32_t latest_open_pressed_micros = 0;

volatile uint8_t close_pressed_count = 0;
volatile uint32_t first_close_pressed_micros = 0;
volatile uint32_t latest_close_pressed_micros = 0;

void loop() {
  auto open_pressed_count_copy = open_pressed_count;
  auto first_open_pressed_micros_copy = first_open_pressed_micros;
  if (open_pressed_count_copy > 0) {
    auto now = micros();
    if (kTestMode == kCountBounces) {
      noInterrupts();
      auto first_open_pressed_micros_copy = first_open_pressed_micros;
      interrupts();
      if (now - first_open_pressed_micros_copy > 500) {
        detachInterrupt(kCoverOpenInterruptNumber);
      }
    } else {
      detachInterrupt(kCoverOpenInterruptNumber);
    }
    Serial.print("open_pressed_count: ");
    Serial.println(open_pressed_count_copy);
    Serial.print("first_open_pressed_micros: ");
    Serial.println(first_open_pressed_micros);
  }

  auto close_pressed_count_copy = close_pressed_count;
  if (close_pressed_count_copy > 0) {
    auto now = micros();
    if (kTestMode == kCountBounces) {
      noInterrupts();
      auto first_close_pressed_micros_copy = first_close_pressed_micros;
      interrupts();
      if (now - first_close_pressed_micros_copy > 500) {
        detachInterrupt(kCoverCloseInterruptNumber);
      }
    } else {
      detachInterrupt(kCoverCloseInterruptNumber);
    }
    Serial.print("close_pressed_count: ");
    Serial.println(close_pressed_count_copy);
    Serial.print("first_close_pressed_micros: ");
    Serial.println(first_close_pressed_micros);
  }
}
