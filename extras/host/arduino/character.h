#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_CHARACTER_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_CHARACTER_H_

// The subset of Arduino's WCharacter.h that I need.

bool isPrintable(const char c);
bool isAlphaNumeric(const char c);
bool isUpperCase(const char c);

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_CHARACTER_H_
