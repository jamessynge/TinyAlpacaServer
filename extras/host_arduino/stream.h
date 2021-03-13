#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_STREAM_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_STREAM_H_

// Declares the subset of Arduino's Stream needed for Tiny Alpaca Server.

#include "extras/host_arduino/print.h"

class Stream : public Print {
 public:
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;

  // Many other methods omitted that are in Arduino's Stream.
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_STREAM_H_
