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
}

void loop() {}
