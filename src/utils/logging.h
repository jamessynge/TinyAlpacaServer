#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_

// Logging and assert-like utility macros for Tiny Alpaca Server. These are
// inspired by the Google Logging Library, but are not identical. The features
// available are controlled by defining, or not, the macro TAS_ENABLE_CHECK, and
// by the value of TAS_ENABLED_VLOG_LEVEL (undefined or defined to an integer in
// the range 1 through 9).
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
// message values will be evaluated. This means that you must not place calls to
// mutating production code into a TAS_CHECK and rely on it being called; i.e.
// don't do something like:
//
//    TAS_CHECK(InitializeHardware()) << "Failed to initialize hardware.";
//
// UNLESS you're planning to always leave TAS_CHECK_ENABLED defined for your
// production (embedded) environment, which is OK if you've kept your use of
// these macros (and hence the messages strings which may consume a lot of
// memory).
//
// TAS_CHECK_NE, TAS_CHECK_EQ, etc. expand to a TAS_CHECK macro with the named
// comparison.
//
///////////////////////////////////////////////////////////////////////////////
//
// TAS_DCHECK Usage:
//
// TAS_DCHECK(expression) << val1 << val2 << val3;
//
// Provides an assert like feature, just like TAS_CHECK, but can be disabled
// while leaving TAS_CHECK enabled. Thus you can place many TAS_DCHECKs in your
// code for debugging, but then disable all of them while still leaving some
// critical TAS_CHECKs enabled.

#include "utils/log_sink.h"
#include "utils/utils_config.h"

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

// TODO(jamessynge): Decide whether to use TASLIT (from inline_literal.h) here
// for the message.

#ifdef TAS_ENABLE_CHECK

#define TAS_CHECK_INTERNAL_(expression, message) \
  if ((expression))                              \
    ;                                            \
  else                                           \
    ::alpaca::CheckSink(message)

#else  // !TAS_ENABLE_CHECK

#define TAS_CHECK_INTERNAL_(expression, message) \
  if (true || (expression))                      \
    ;                                            \
  else                                           \
    ::alpaca::VoidSink()

#endif  // TAS_ENABLE_CHECK

#define TAS_CHECK(expression) TAS_CHECK_INTERNAL_(expression, #expression)
#define TAS_CHECK_EQ(a, b) TAS_CHECK_INTERNAL_((a) == (b), #a " == " #b)
#define TAS_CHECK_LE(a, b) TAS_CHECK_INTERNAL_((a) <= (b), #a " <= " #b)
#define TAS_CHECK_LT(a, b) TAS_CHECK_INTERNAL_((a) < (b), #a " < " #b)
#define TAS_CHECK_GE(a, b) TAS_CHECK_INTERNAL_((a) >= (b), #a " >= " #b)
#define TAS_CHECK_GT(a, b) TAS_CHECK_INTERNAL_((a) > (b), #a " > " #b)

// It is common that when operator== is defined for a pair of types,
// operator!= is not defined for that same pair of types. On that
// basis we implement TAS_CHECK_NE by negating ==.
#define TAS_CHECK_NE(a, b) \
  TAS_CHECK_INTERNAL_(!((a) == (b)), "!(" #a " == " #b ")")

// If the TAS_CHECK* macros are enabled, then TAS_DCHECK* macros may also be
// enabled, but if TAS_CHECK* macros are disabled, then the TAS_DCHECK* macros
// are also disabled.

#if defined(TAS_ENABLE_CHECK) && defined(TAS_ENABLE_DCHECK)

#define TAS_DCHECK_INTERNAL_(expression, message) \
  TAS_CHECK_INTERNAL_(expression, message)

#else

#define TAS_DCHECK_INTERNAL_(expression, message) \
  if (true || (expression))                       \
    ;                                             \
  else                                            \
    ::alpaca::VoidSink()

#endif

#define TAS_DCHECK(expression) TAS_DCHECK_INTERNAL_(expression, #expression)
#define TAS_DCHECK_EQ(a, b) TAS_DCHECK_INTERNAL_((a) == (b), #a " == " #b)
#define TAS_DCHECK_LE(a, b) TAS_DCHECK_INTERNAL_((a) <= (b), #a " <= " #b)
#define TAS_DCHECK_LT(a, b) TAS_DCHECK_INTERNAL_((a) < (b), #a " < " #b)
#define TAS_DCHECK_GE(a, b) TAS_DCHECK_INTERNAL_((a) >= (b), #a " >= " #b)
#define TAS_DCHECK_GT(a, b) TAS_DCHECK_INTERNAL_((a) > (b), #a " > " #b)

#define TAS_DCHECK_NE(a, b) \
  TAS_DCHECK_INTERNAL_(!((a) == (b)), "!(" #a " == " #b ")")

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
