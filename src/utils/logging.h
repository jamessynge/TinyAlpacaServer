#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_

// To enable logging (for debugging) when compiled to run on a host, we use
// these TAS_* log macros to define our log statements. On Arduino, they're
// no-ops, but on the host they redirect to the Google defined logging macros,
// which are available publicly in several Google published libraries, such as:
// Google Logging Library's <glog/logging.h>
// TensorFlow's core/platform/default/logging.h.
//
// Author: james.synge@gmail.com

#include "utils/log_sink.h"

#ifdef ARDUINO

#define TAS_LOG(severity, ...)
#define TAS_VLOG(level, ...)
#define TAS_DLOG(severity, ...)
#define TAS_DVLOG(level, ...)

#if VA_OPT_SUPPORTED

// TAS_CHECK is NOT yet working for Arduino AVR; it also needs to call a
// function that goes into an infinite loop reporting that a failure has
// occurred periodically. The other TAS_CHECK methods need to call into this
// method, though that really means that there should be a shared helper macro.
#define TAS_CHECK(expression, ...)                       \
  if ((expression)) {                                    \
  } else {                                               \
    alpaca::OPrintStream _o_print_stream_(Serial);       \
    _o_print_stream_ << "CHECK failed: " << #expression; \
    __VA_OPT__(_o_print_stream_ << "\n" <<)              \
    __VA_ARGS__;                                         \
  }

#else
#define TAS_CHECK(expression, ...)
#endif

#define TAS_CHECK_EQ(a, b, ...)
#define TAS_CHECK_NE(a, b, ...)
#define TAS_CHECK_LE(a, b, ...)
#define TAS_CHECK_LT(a, b, ...)
#define TAS_CHECK_GE(a, b, ...)
#define TAS_CHECK_GT(a, b, ...)

#define TAS_DCHECK(expression, ...)
#define TAS_DCHECK_EQ(a, b, ...)
#define TAS_DCHECK_NE(a, b, ...)
#define TAS_DCHECK_LE(a, b, ...)
#define TAS_DCHECK_LT(a, b, ...)
#define TAS_DCHECK_GE(a, b, ...)
#define TAS_DCHECK_GT(a, b, ...)

#else  //! ARDUINO

#include "logging.h"

#define TAS_LOG(severity, ...) LOG(severity) __VA_OPT__(<<) __VA_ARGS__
#define TAS_VLOG(level, ...) VLOG(level) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DLOG(severity, ...) DLOG(severity) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DVLOG(level, ...) DVLOG(level) __VA_OPT__(<<) __VA_ARGS__

#define TAS_CHECK(expression, ...) CHECK(expression) __VA_OPT__(<<) __VA_ARGS__
#define TAS_CHECK_EQ(a, b, ...) CHECK_EQ(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_CHECK_NE(a, b, ...) CHECK_NE(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_CHECK_LE(a, b, ...) CHECK_LE(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_CHECK_LT(a, b, ...) CHECK_LT(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_CHECK_GE(a, b, ...) CHECK_GE(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_CHECK_GT(a, b, ...) CHECK_GT(a, b) __VA_OPT__(<<) __VA_ARGS__

#define TAS_DCHECK(expression, ...) \
  DCHECK(expression) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DCHECK_EQ(a, b, ...) DCHECK_EQ(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DCHECK_NE(a, b, ...) DCHECK_NE(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DCHECK_LE(a, b, ...) DCHECK_LE(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DCHECK_LT(a, b, ...) DCHECK_LT(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DCHECK_GE(a, b, ...) DCHECK_GE(a, b) __VA_OPT__(<<) __VA_ARGS__
#define TAS_DCHECK_GT(a, b, ...) DCHECK_GT(a, b) __VA_OPT__(<<) __VA_ARGS__

#endif  // ARDUINO

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOGGING_H_
