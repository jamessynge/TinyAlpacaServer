#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_

// Logging and assert-like utility macros for Tiny Alpaca Server.
// These are inspired by the Google Logging Library, but are not identical.
// The features available are controlled by defining, or not, the macro
// TAS_ENABLE_CHECK, and by the value of TAS_ENABLED_VLOG_LEVEL (undefined
// or defined to an integer in the range 1 through 9).
//
// Author: james.synge@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
//
// TAS_VLOG Usage:
//
// TAS_VLOG(level) << val1 << val2 << val3;
//
// Will emit a line of text with the string representations of val1, val2,
// and val3 concatenated together, and terminated by a newline, if level is
// less than or equal to the value of TAS_ENABLED_VLOG_LEVEL; if the level
// is greater than the value of TAS_ENABLED_VLOG_LEVEL, or if it is undefined,
// then no message is emitted, and generally speaking the statement will be
// omitted from the compiled binary.
//
///////////////////////////////////////////////////////////////////////////////
//
// TAS_CHECK Usage:
//
// TAS_CHECK(expression) << val1 << val2 << val3;
//
// If TAS_ENABLE_CHECK is defined, then the expression will be evaluated; if the
// expression is is true, no message is emitted and the statement completes
// normally. However if the statement evaluates to false, this will emit a
// message will emit a line of text with the string representations of val1,
// val2, and val3 concatenated together, terminated by a newline, and the
// program will not continue. On the Arduino, it will endlessly loop producing
// an error message; on the host it will exit the program.
//
// If TAS_ENABLE_CHECK is not defined, then neither the expression nor the
// message values will be evaluated.

#include "utils/log_sink.h"

#define TAS_VLOG(level) TAS_VLOG_LEVEL_##level
#define TAS_VLOG_LEVEL_1 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_2 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_3 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_4 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_5 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_6 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_7 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_8 ::alpaca::VoidSink()
#define TAS_VLOG_LEVEL_9 ::alpaca::VoidSink()

#ifdef TAS_ENABLED_VLOG_LEVEL

#if TAS_ENABLED_VLOG_LEVEL >= 1
#undef TAS_VLOG_LEVEL_1
#define TAS_VLOG_LEVEL_1 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 1

#if TAS_ENABLED_VLOG_LEVEL >= 2
#undef TAS_VLOG_LEVEL_2
#define TAS_VLOG_LEVEL_2 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 2

#if TAS_ENABLED_VLOG_LEVEL >= 3
#undef TAS_VLOG_LEVEL_3
#define TAS_VLOG_LEVEL_3 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 3

#if TAS_ENABLED_VLOG_LEVEL >= 4
#undef TAS_VLOG_LEVEL_4
#define TAS_VLOG_LEVEL_4 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 4

#if TAS_ENABLED_VLOG_LEVEL >= 5
#undef TAS_VLOG_LEVEL_5
#define TAS_VLOG_LEVEL_5 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 5

#if TAS_ENABLED_VLOG_LEVEL >= 6
#undef TAS_VLOG_LEVEL_6
#define TAS_VLOG_LEVEL_6 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 6

#if TAS_ENABLED_VLOG_LEVEL >= 7
#undef TAS_VLOG_LEVEL_7
#define TAS_VLOG_LEVEL_7 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 7

#if TAS_ENABLED_VLOG_LEVEL >= 8
#undef TAS_VLOG_LEVEL_8
#define TAS_VLOG_LEVEL_8 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 8

#if TAS_ENABLED_VLOG_LEVEL >= 9
#undef TAS_VLOG_LEVEL_9
#define TAS_VLOG_LEVEL_9 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 9

#endif  // TAS_ENABLED_VLOG_LEVEL

#ifdef TAS_ENABLE_CHECK

#define TAS_CHECK(expression) \
  if ((expression))           \
    ;                         \
  else                        \
    ::alpaca::CheckSink()

#else  // !TAS_ENABLE_CHECK

#define TAS_CHECK(expression) \
  if (true || (expression))   \
    ;                         \
  else                        \
    ::alpaca::VoidSink()

#endif  // TAS_ENABLE_CHECK

#define TAS_CHECK_EQ(a, b) TAS_CHECK((a) == (b))
#define TAS_CHECK_LE(a, b) TAS_CHECK((a) <= (b))
#define TAS_CHECK_LT(a, b) TAS_CHECK((a) < (b))
#define TAS_CHECK_GE(a, b) TAS_CHECK((a) >= (b))
#define TAS_CHECK_GT(a, b) TAS_CHECK((a) > (b))
// It is common that when operator== is defined for a pair of types,
// operator!= is not defined for that same pair of types. On that
// basis we implement TAS_CHECK_NE by negating ==.
#define TAS_CHECK_NE(a, b) TAS_CHECK(!((a) == (b)))

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
