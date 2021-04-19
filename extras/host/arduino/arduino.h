#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ARDUINO_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ARDUINO_H_

// This provides just (?) enough of Arduino.h for my needs when testing on host.
//
// As of mid-April 2021, I'm starting to write code that needs to read and write
// pins, so may need to add some more support for fake hardware (e.g. a
// mock-able class).

// These standard libraries are included directly or indirectly by Arduino.h.

#include <ctype.h>    // IWYU pragma: export
#include <math.h>     // IWYU pragma: export
#include <stdbool.h>  // IWYU pragma: export
#include <stdlib.h>   // IWYU pragma: export
#include <string.h>   // IWYU pragma: export
// #include <inttypes.h>  // Handled via int_types.h below, which provides the
// subset I use.

// These AVR specific libraries are included directly or indirectly by
// Arduino.h.

#include "extras/host/arduino/pgmspace.h"  // IWYU pragma: export

// #include avr/io   // Not needed (or implemented) on host.
// #include avr/interrupt  // Not needed on host.

// These Arduino specific libraries are included directly or indirectly by
// Arduino.h.

// #include "Binary.h"  // Not needed in my code.

// #include "WCharacter.h" provides wrappers around <ctype.h>, so I should use
// those directly. On the other hand, the ctype impl is a bunch of assembly
// language code rather than a lookup table. IFF short on flash, consider
// whether it would be smaller if I wrote my own for the few functions I need
// while decoding a request.
#include "extras/host/arduino/character.h"  // IWYU pragma: export
#include "extras/host/arduino/int_types.h"  // IWYU pragma: export

// Arduino's HardwareSerial.h includes Stream.h, which in turn includes Print.h.
// I'm explicitly including them here to make it easier to work with IWYU.
#include "extras/host/arduino/print.h"   // IWYU pragma: export
#include "extras/host/arduino/random.h"  // IWYU pragma: export
#include "extras/host/arduino/serial.h"  // IWYU pragma: export
#include "extras/host/arduino/stream.h"  // IWYU pragma: export

uint32_t millis();
uint32_t micros();

void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

#define INPUT 97
#define INPUT_PULLUP 98
#define OUTPUT 99
#define pinMode(pin_number, pin_mode)

#define LOW 0
#define HIGH 1
#define digitalWrite(pin_number, value)

#define analogWrite(pin_number, value)

#define digitalRead(pin_number) HIGH

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_ARDUINO_H_
