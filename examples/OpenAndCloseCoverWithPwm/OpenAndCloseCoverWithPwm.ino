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

// -----------------------------------------------------------------------------
//
// Pin selection analysis:
//
// For the limit switches, we've already selected pins that can be used for
// triggering an interrupt when one of those pins is held low AND the External
// Interrupt Control Register A (for INT0 through INT3) enables that pin to
// interrupt. This means that we should do the following when preparing to close
// the cover:
//
// 1) Set volatile field current_action_ that indicates we're moving the cover
//    towards closed (e.g. an enum field, where the enum has enumerators
//    kNotMoving, kOpening, kClosing).
// 2) Enable the "pin low" interrupt for detecting that the cover is closed
//    (e.g. INT0 is LOW). When that interrupt is trigged, it should set field
//    current_action_ to kNotMoving and then should disable itself; note that it
//    is possible that the switch is bouncy, and may first indicate the limit is
//    reached slightly earlier (see below); it that turns out to be true, we may
//    need to debounce in either the interrupt handler, or in some main loop()
//    code.
// 3) Configure PWM or a timer/counter with interrupt on overflow or match; the
//    latter would have the benefit that the PWM pulse isn't automatically
//    generated, but instead allows a small amount of logic to decide whether to
//    emit the pulse, including doing debouncing before disabling the timer.

// Arduino Mega pins 11 and 12, i.e. PB5 and PB6, can be used as 16-bit T/C 1
// outputs OC1A and OC1B, can also be used as Pin Change Interrupts.

// Limit switches:
//
// The limit switches are intended to tell the software when it should stop
// moving the cover, and at startup they could be used to determine whether the
// cover should be closed. I assume that the switches may be bouncy, and may
// trigger before the cover has reached the full limit. Furthermore, both
// switches are located near the hinge rather than near the far side of the
// cover, so they're likely to be rather sensitive to exact placement: if you
// move them a little closer to the cover, they'll trigger to soon; if you move
// them a little farther away, they may sometimes fail to trigger.
//
// Testing will be needed to determine exactly when the switches trigger and
// whether they're well placed.

// Movement limits:
//
// The prototype Cover-Calibrator uses this stepper motor with a 90.25:1
// reduction gear:
//
//   https://www.omc-stepperonline.com/nema-8-stepper-motor-bipolar-l-38mm-w-gear-ratio-90-1-planetary-gearbox.html
//
// A full step is 1.8 degress, and the Arduino Shield we're using configures the
// stepper driver to do 1/8th microsteps. To rotate 270 degress (i.e. fully open
// to fully closed, or visa versa), requires 108300 microsteps:
//
//   270 / 1.8 * 8 * 90.25 = 108300
//
// In addition to using the limit switches, it is probably a good idea to stop
// or otherwise alert if the number of steps taken is sufficient beyond the
// expected number.
//
// The stepper driver chip requires that a pulse be HIGH for at least 1
// microsecond, followed by LOW for at least 1 microsecond. If the stepper
// driver and stepper could move that fast, we'd be able to move 500K 1/8
// microsteps per second, i.e. a full sweep of the cover would take just over
// 2/10 of a second, dangerously fast.
//
// Alan's prototype sketch configured AccelStepper to run at 20K steps per
// second, or around 5.25 seconds to open and close; the sketch did not use the
// acceleration and deceleration features of AccelStepper, so it isn't clear to
// me how fast we can actually drive the motor and cover.

namespace {
constexpr uint8_t cover_open_interrupt =
    digitalPinToInterrupt(kCoverOpenLimitPin);
constexpr uint8_t cover_close_interrupt =
    digitalPinToInterrupt(kCoverCloseLimitPin);

int open_pressed_count = 0;
int close_pressed_count = 0;

void OpenPressed() {
  ++open_pressed_count;
  detachInterrupt(cover_open_interrupt);
}

void ClosePressed() {
  ++close_pressed_count;
  detachInterrupt(cover_close_interrupt);
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

  pinMode(kCoverOpenLimitPin, INPUT_PULLUP);
  pinMode(kCoverCloseLimitPin, INPUT_PULLUP);
}

void loop() {}
