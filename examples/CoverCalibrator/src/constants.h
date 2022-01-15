#ifndef TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_CONSTANTS_H_
#define TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_CONSTANTS_H_

// Pin constants, etc. Note that the enable pins are grounded to indicate that
// the feature is present.

#define kNoSuchPin 255  // Enable pins are allowed to be omitted.

#if 0

// Based on AM_CoverCalibrator_schematic_rev_5_pcb.pdf, which has more detail
// than revision 6, though in revision 6 there is no LED 1 enabled pin.

#define kLedChannel1PwmPin 5
#define kLedChannel1EnabledPin kNoSuchPin

#define kLedChannel2PwmPin 6
#define kLedChannel2EnabledPin 10

#define kLedChannel3PwmPin 7  // Conflicts with W5500 chip select for SPI.
#define kLedChannel3EnabledPin 11

#define kLedChannel4PwmPin 8
#define kLedChannel4EnabledPin 12

#define kCoverMotorStepPin 3
#define kCoverMotorDirectionPin 4  // Conflicts with SDcard chip select for SPI.
#define kCoverOpenLimitPin 20
#define kCoverCloseLimitPin 21
#define kCoverEnabledPin \
  kNoSuchPin  // really 13, preserving builtin LED access.

#elif 0

// Suggested pin selection for next revision after rev 6.
//
// The Microstep Resolution pins are optional, but seem like a good idea; if not
// provided, I recommend at least providing jumpers, though making this software
// configurable seems better.
//
// I recommend including the enabled pin for LED channel 1 as this will make it
// easier for the code to provide a sensible default label for the switch that
// enables each channel (e.g. "LED Channel 1 (UV)", "LED Channel 3 (Near IR)"),
// and for a user to choose to disable channel 1 by pulling a jumper cable,
// rather than by rewiring channels so that 2 becomes 1, and so on.

//                                        Alt. Func.       Rev. 6 pin  Dir
#define kLedChannel1PwmPin 2           // OC3B             was 5       Output
#define kLedChannel2PwmPin 3           // OC3C             was 6       Output
#define kLedChannel3PwmPin 5           // OC3A             was 7       Output
#define kLedChannel4PwmPin 6           // OC4A             was 8       Output
#define kLedChannel1EnabledPin PIN_A0  // ADC0             NEW         Input
#define kLedChannel2EnabledPin PIN_A1  // ADC1             was 10      Input
#define kLedChannel3EnabledPin PIN_A2  // ADC2             was 11      Input
#define kLedChannel4EnabledPin PIN_A3  // ADC3             was 12      Input
#define kCoverMotorStepPin 16          // TXD2             was 3       Output
#define kCoverMotorDirectionPin 17     // RXD2             was 4       Output
#define kCoverOpenLimitPin 18          // TXD1, INT3       was 20      Input
#define kCoverCloseLimitPin 19         // RXD1, INT2       was 21      Input
#define kCoverEnabledPin PIN_A8        // ADC8, PCINT16    was 13      Input
#define kMicrostepResolution1 PIN_A9   // ADC9, PCINT17    NEW         Output
#define kMicrostepResolution2 PIN_A10  // ADC10, PCINT18   NEW         Output
#define kMicrostepResolution3 PIN_A11  // ADC11, PCINT19   NEW         Output

#else
// Based on AM_CoverCalibrator_schematic_rev_8_pcb.pdf, for the board labeled
// "Cap Calibrator Rev2 austinb, Alan Sliski", manufactured at the end of
// December, 2021... with corrections based on connectivity tests of the board:
//
// * The schematic claims that A13 is connected to the ~ENABLE pin of the A4988
//   carrier board. Instead it appears to just connect to the "Motorized Cap
//   Enable" jumper.
//
// * The A9 and A11 lines are swapped, connecting to MS3 and MS1.

//                                        Alt. Func.       Rev. 6 pin  Dir
#define kLedChannel1PwmPin 2           // OC3B             was 5       Output
#define kLedChannel2PwmPin 3           // OC3C             was 6       Output
#define kLedChannel3PwmPin 5           // OC3A             was 7       Output
#define kLedChannel4PwmPin 6           // OC4A             was 8       Output
#define kLedChannel1EnabledPin PIN_A0  // ADC0             NEW         Input
#define kLedChannel2EnabledPin PIN_A1  // ADC1             was 10      Input
#define kLedChannel3EnabledPin PIN_A2  // ADC2             was 11      Input
#define kLedChannel4EnabledPin PIN_A3  // ADC3             was 12      Input
#define kCoverMotorStepPin 16          // TXD2             was 3       Output
#define kCoverMotorDirectionPin 17     // RXD2             was 4       Output
#define kCoverOpenLimitPin 18          // TXD1, INT3       was 20      Input
#define kCoverCloseLimitPin 19         // RXD1, INT2       was 21      Input
#define kCoverEnabledPin PIN_A13       // ADC13, PCINT21   was 13      Input
#define kMicrostepResolution3 PIN_A9   // ADC9, PCINT17    NEW         Output
#define kMicrostepResolution2 PIN_A10  // ADC10, PCINT18   NEW         Output
#define kMicrostepResolution1 PIN_A11  // ADC11, PCINT19   NEW         Output

#endif

// TODO(jamessynge): Figure out which of these to make run-time configurable
// parameters (e.g. stored in EEPROM).

#define kDeviceEnabledPinMode INPUT_PULLUP

#define kDirectionOpen LOW
#define kDirectionClose HIGH

#define kCoverPresentPinMode kDeviceEnabledPinMode
#define kCoverIsPresent LOW

#define kLimitSwitchPinMode INPUT_PULLUP
#define kLimitSwitchClosed LOW

#define kStepsPerSecond 10000
#define kMaximumSteps 120000
#define kMaximumStartSteps 5000

#define kMaxBrightness 0xffff  // We're using a 16-bit Timer/Counter.

#endif  // TINY_ALPACA_SERVER_EXAMPLES_COVERCALIBRATOR_SRC_CONSTANTS_H_
