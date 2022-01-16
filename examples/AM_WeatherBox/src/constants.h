#ifndef TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_CONSTANTS_H_
#define TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_CONSTANTS_H_

// Pin constants, etc.
//
// NOTE: I'm not supporting multiple AM Weather Box devices, just a single one
// using the standard SDA/SCL pins for communicating with a single MLX90614, and
// a single RG11 Rain Sensor.
//
// Author: james.synge@gmail.com

#include <Arduino.h>

#define kNoSuchPin 255  // Enable pins are allowed to be omitted.

#if defined(SDA) && defined(SCL)

#define kMlx90614SdaPin SDA
#define kMlx90614SclPin SCL

#elif defined(PIN_WIRE_SDA) && defined(PIN_WIRE_SCL)

#define kMlx90614SdaPin PIN_WIRE_SDA
#define kMlx90614SclPin PIN_WIRE_SCL

#else

#error "Two-Wire data and clock lines not configured."

#define kMlx90614SdaPin kNoSuchPin
#define kMlx90614SclPin kNoSuchPin

#endif

#define kRg11SensorPin 47
#define kRg11SensorPinMode INPUT_PULLUP
#define kRg11DetectsRain LOW

#endif  // TINY_ALPACA_SERVER_EXAMPLES_AM_WEATHERBOX_SRC_CONSTANTS_H_
