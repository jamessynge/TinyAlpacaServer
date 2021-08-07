#ifndef TINY_ALPACA_SERVER_SRC_UTILS_STREAM_TO_PRINT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_STREAM_TO_PRINT_H_

// Enables streaming output to Arduino Print instances, including Serial, thus
// simplifying writing code that produces output from multiple items.
//
// This is in a separate header file from o_print_stream.h so that it doesn't
// interfere with the use of any other operator<< whose left-hand-side is a
// Print instance.
//
// Author: james.synge@gmail.com

#include "mcucore_platform.h"
#include "o_print_stream.h"

template <typename T>
alpaca::OPrintStream operator<<(Print& out, const T& value) {
  alpaca::OPrintStream strm(out);
  strm << value;
  return strm;
}

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_STREAM_TO_PRINT_H_
