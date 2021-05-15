#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_CONSTANTS_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_CONSTANTS_H_

// Pin constants, etc.

#if 1

// Based on AM_CoverCalibrator_schematic_rev_5_pcb.pdf, which has more detail
// than revision 6, though in revision 6 there is no LED 1 enabled pin.

#define kLedChannel1PwmPin 5
#define kLedChannel1EnabledPin 9

#define kLedChannel2PwmPin 6
#define kLedChannel2EnabledPin 10

#define kLedChannel3PwmPin 7
#define kLedChannel3EnabledPin 11

#define kLedChannel4PwmPin 8
#define kLedChannel4EnabledPin 12

#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 4
#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21
#define kCoverEnabledPin 13

#else

// Suggested pin selection for next revision.

#define kLedChannel1PwmPin 2           // OC3B
#define kLedChannel2PwmPin 3           // OC3C
#define kLedChannel3PwmPin 5           // OC3A
#define kLedChannel4PwmPin 6           // OC4A
#define kLedChannel2EnabledPin PIN_A1  //              was 10
#define kLedChannel3EnabledPin PIN_A2  //              was 11
#define kLedChannel4EnabledPin PIN_A3  //              was 12
#define kCoverEnabledPin PIN_A4        //              was 13
#define kCoverMotorStepPin 16          // TXD2         was 3
#define kCoverMotorDirectionPin 17     // RXD2         was 4
#define kCoverOpenLimitPin 18          // TXD1, INT3   was 20
#define kCoverCloseLimitPin 19         // RXD1, INT2   was 21

#endif

// TODO(jamessynge): Figure out which of these to make run-time configurable
// parameters (e.g. stored in EEPROM).

#define kDirectionOpen LOW
#define kDirectionClose HIGH

#define kCoverPresentPinMode INPUT_PULLUP
#define kCoverIsPresent LOW

#define kLimitSwitchPinMode INPUT_PULLUP
#define kLimitSwitchClosed LOW

#define kStepsPerSecond 10000
#define kMaximumSteps 120000
#define kMaximumStartSteps 5000

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_CONSTANTS_H_
