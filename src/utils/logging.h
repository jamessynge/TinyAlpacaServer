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
// TAS_CHECK(expression) << message << values;
//
// The expression is always evaluated, but the result is only examined by the
// macro if TAS_ENABLE_CHECK is defined.
//
// In all cases, if expression is true, then no message is emitted and the
// statement completes normally (e.g. the next statement is executed or the
// scope is exited, as appropriate).
//
// When TAS_ENABLE_CHECK is defined and the expression is false, TAS_CHECK will
// emit a line of text indicating that the expression has failed, followed by
// the string representations of the message values inserted into TAS_CHECK, and
// finally a newline. On the host, the program will exit; on the Arduino, it
// will endlessly loop producing an error message; on the host it will exit the
// program.
//
// If TAS_ENABLE_CHECK is not defined and the expression is false, it will be
// treated as if true. This allows one to include code such as the following,
// and know that InitializeHardware will always be called when that statement is
// reached, regardless of whether TAS_ENABLE_CHECK is defined:
//
//     TAS_CHECK(InitializeHardware())
//         << TAS_FLASHSTR("Failed to initialize hardware.");
//
// This allows you to make extensive use of TAS_CHECK, yet know that the
// compiled size of the statement will shrink to that of the expression when
// TAS_ENABLE_CHECK is not defined.
//
// Note that we use the TAS_FLASHSTR macro here so that when compiling for AVR
// microcontrollers the string is stored only in Flash (PROGMEM), and is not
// copied to RAM.
//
// TAS_CHECK_NE, TAS_CHECK_EQ, etc. expand to a TAS_CHECK macro with the named
// comparison.
//
///////////////////////////////////////////////////////////////////////////////
//
// TAS_DCHECK Usage:
//
// TAS_DCHECK(expression) << message << values;
//
// Provides an assert like feature, similar to TAS_CHECK, but the expression is
// evaluated only if both TAS_ENABLE_CHECK and TAS_ENABLE_DCHECK are defined.
// Thus you can place many TAS_DCHECKs in your code for debugging, but then
// disable all of them while still leaving some critical TAS_CHECKs enabled.
//
// TAS_DCHECK_NE, TAS_DCHECK_EQ, etc. expand to a TAS_DCHECK macro with the
// named comparison.

#include "utils/basename.h"
#include "utils/log_sink.h"
#include "utils/utils_config.h"

#define THE_VOID_SINK ::alpaca::VoidSink()

#if defined(TAS_ENABLED_VLOG_LEVEL) && TAS_ENABLED_VLOG_LEVEL > 0

#define TAS_VLOG(level)              \
  switch (0)                         \
  default:                           \
    (TAS_ENABLED_VLOG_LEVEL < level) \
        ? (void)0                    \
        : ::alpaca::LogSinkVoidify() && ::alpaca::LogSink()

#define TAS_IS_VLOG_ON(level) ((level) >= TAS_ENABLED_VLOG_LEVEL)

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
extern void [[TAS_ENABLED_VLOG_LEVEL_is(TAS_ENABLED_VLOG_LEVEL)]] SomeFuncA();
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#else

#define TAS_VLOG(level) \
  switch (0)            \
  default:              \
    (true) ? (void)0 : ::alpaca::LogSinkVoidify() && THE_VOID_SINK

#define TAS_IS_VLOG_ON(level) (false)

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
extern void [[TAS_VLOG_uses_THE_VOID_SINK]] SomeFuncA();
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#endif

// TODO(jamessynge): Consider adding TAS_ENABLE_NO_LOG_CHECK, which would log
// just the line number of a failed CHECK, rather than the full message, thus
// saving (a lot of?) PROGMEM used for storing message strings. This will make
// it harder to track down a failure, but not impossible.

#ifdef TAS_ENABLE_CHECK

#define TAS_CHECK_INTERNAL_(expression, message)                             \
  switch (0)                                                                 \
  default:                                                                   \
    (expression) ? (void)0                                                   \
                 : ::alpaca::LogSinkVoidify() &&                             \
                       ::alpaca::CheckSink(TAS_BASENAME(__FILE__), __LINE__, \
                                           TAS_FLASHSTR(message))

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
extern void [[TAS_ENABLE_CHECK_is_defined]] SomeFuncB();
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#else  // !TAS_ENABLE_CHECK

#define TAS_CHECK_INTERNAL_(expression, message) \
  switch (0)                                     \
  default:                                       \
    ((expression) || true) ? (void)0             \
                           : ::alpaca::LogSinkVoidify() && THE_VOID_SINK

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
extern void [[TAS_ENABLE_CHECK_is_NOT_defined]] SomeFuncB();
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#endif  // TAS_ENABLE_CHECK

#define TAS_CHECK(expression) TAS_CHECK_INTERNAL_(expression, #expression)
#define TAS_CHECK_EQ(a, b) TAS_CHECK_INTERNAL_((a) == (b), #a " == " #b)
#define TAS_CHECK_NE(a, b) TAS_CHECK_INTERNAL_((a) != (b), #a " != " #b)
#define TAS_CHECK_LE(a, b) TAS_CHECK_INTERNAL_((a) <= (b), #a " <= " #b)
#define TAS_CHECK_LT(a, b) TAS_CHECK_INTERNAL_((a) < (b), #a " < " #b)
#define TAS_CHECK_GE(a, b) TAS_CHECK_INTERNAL_((a) >= (b), #a " >= " #b)
#define TAS_CHECK_GT(a, b) TAS_CHECK_INTERNAL_((a) > (b), #a " > " #b)

// If the TAS_CHECK* macros are enabled, then TAS_DCHECK* macros may also be
// enabled, but if TAS_CHECK* macros are disabled, then the TAS_DCHECK* macros
// are also disabled.

#if defined(TAS_ENABLE_CHECK) && defined(TAS_ENABLE_DCHECK)

#define TAS_DCHECK_INTERNAL_(expression, message)                            \
  switch (0)                                                                 \
  default:                                                                   \
    (expression) ? (void)0                                                   \
                 : ::alpaca::LogSinkVoidify() &&                             \
                       ::alpaca::CheckSink(TAS_BASENAME(__FILE__), __LINE__, \
                                           TAS_FLASHSTR(message))

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
extern void [[TAS_ENABLE_DCHECK_is_defined]] SomeFuncC();
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#else

#ifdef TAS_ENABLE_DCHECK
#undef TAS_ENABLE_DCHECK
#endif

#define TAS_DCHECK_INTERNAL_(expression, message) \
  switch (0)                                      \
  default:                                        \
    (true || (expression)) ? (void)0              \
                           : ::alpaca::LogSinkVoidify() && THE_VOID_SINK

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
extern void [[TAS_ENABLE_DCHECK_is_NOT_defined]] SomeFuncC();
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#endif

#define TAS_DCHECK(expression) TAS_DCHECK_INTERNAL_(expression, #expression)
#define TAS_DCHECK_EQ(a, b) TAS_DCHECK_INTERNAL_((a) == (b), #a " == " #b)
#define TAS_DCHECK_NE(a, b) TAS_DCHECK_INTERNAL_((a) != (b), #a " != " #b)
#define TAS_DCHECK_LE(a, b) TAS_DCHECK_INTERNAL_((a) <= (b), #a " <= " #b)
#define TAS_DCHECK_LT(a, b) TAS_DCHECK_INTERNAL_((a) < (b), #a " < " #b)
#define TAS_DCHECK_GE(a, b) TAS_DCHECK_INTERNAL_((a) >= (b), #a " >= " #b)
#define TAS_DCHECK_GT(a, b) TAS_DCHECK_INTERNAL_((a) > (b), #a " > " #b)

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
