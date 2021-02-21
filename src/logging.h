#ifndef TINY_ALPACA_SERVER_LOGGING_H_
#define TINY_ALPACA_SERVER_LOGGING_H_

// To enable logging (for debugging) when compiled to run on a host, we use
// these TAS_* log macros to define our log statements. On Arduino, they're
// no-ops, but on the host they redirect to the Google defined logging macros,
// which are available publicly in the TensorFlow file:
// core/platform/default/logging.h.
//
// Author: james.synge@gmail.com

#ifdef ARDUINO

#define TAS_LOG(severity, msg)
#define TAS_VLOG(level, msg)
#define TAS_DLOG(severity, msg)
#define TAS_DVLOG(level, msg)

#define TAS_CHECK(expression, msg)
#define TAS_CHECK_EQ(a, b, msg)
#define TAS_CHECK_NE(a, b, msg)
#define TAS_CHECK_LE(a, b, msg)
#define TAS_CHECK_LT(a, b, msg)
#define TAS_CHECK_GE(a, b, msg)
#define TAS_CHECK_GT(a, b, msg)

#define TAS_DCHECK(expression, msg)
#define TAS_DCHECK_EQ(a, b, msg)
#define TAS_DCHECK_NE(a, b, msg)
#define TAS_DCHECK_LE(a, b, msg)
#define TAS_DCHECK_LT(a, b, msg)
#define TAS_DCHECK_GE(a, b, msg)
#define TAS_DCHECK_GT(a, b, msg)

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

#endif  // TINY_ALPACA_SERVER_LOGGING_H_
