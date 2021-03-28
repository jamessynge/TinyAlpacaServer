#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/log_sink.h"
#include "utils/logging.h"

#undef TAS_LOG
#undef TAS_VLOG
#undef TAS_DLOG
#undef TAS_DVLOG
#undef TAS_CHECK

#undef TAS_CHECK_EQ
#undef TAS_CHECK_NE
#undef TAS_CHECK_LE
#undef TAS_CHECK_LT
#undef TAS_CHECK_GE
#undef TAS_CHECK_GT

#undef TAS_DCHECK
#undef TAS_DCHECK_EQ
#undef TAS_DCHECK_NE
#undef TAS_DCHECK_LE
#undef TAS_DCHECK_LT
#undef TAS_DCHECK_GE
#undef TAS_DCHECK_GT

#ifdef TAS_ENABLE_CHECK

#define TAS_DCHECK(expression) \
  if ((expression))            \
    ;                          \
  else                         \
    ::alpaca::CheckSink()

#else

#define TAS_DCHECK(expression) \
  if (true || (expression))    \
    ;                          \
  else                         \
    ::alpaca::VoidSink()

#endif

#define TAS_DLOG(level) TAS_DLOG_LEVEL_##level
#define TAS_DLOG_LEVEL_1 ::alpaca::VoidSink()
#define TAS_DLOG_LEVEL_2 ::alpaca::VoidSink()
#define TAS_DLOG_LEVEL_3 ::alpaca::VoidSink()
#define TAS_DLOG_LEVEL_4 ::alpaca::VoidSink()
#define TAS_DLOG_LEVEL_5 ::alpaca::VoidSink()

#ifdef TAS_ENABLED_VLOG_LEVEL

#if TAS_ENABLED_VLOG_LEVEL >= 1
#undef TAS_DLOG_LEVEL_1
#define TAS_DLOG_LEVEL_1 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 1

#if TAS_ENABLED_VLOG_LEVEL >= 2
#undef TAS_DLOG_LEVEL_2
#define TAS_DLOG_LEVEL_2 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 2

#if TAS_ENABLED_VLOG_LEVEL >= 3
#undef TAS_DLOG_LEVEL_3
#define TAS_DLOG_LEVEL_3 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 3

#if TAS_ENABLED_VLOG_LEVEL >= 4
#undef TAS_DLOG_LEVEL_4
#define TAS_DLOG_LEVEL_4 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 4

#if TAS_ENABLED_VLOG_LEVEL >= 5
#undef TAS_DLOG_LEVEL_5
#define TAS_DLOG_LEVEL_5 ::alpaca::LogSink()
#endif  // TAS_ENABLED_VLOG_LEVEL >= 5

#endif  // TAS_ENABLED_VLOG_LEVEL

inline void LogExperimentTestFunction() {
#if TAS_HOST_TARGET
  LOG(INFO) << "LogExperimentTestFunction enter";
#endif

  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_DLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;

  if (true)
    TAS_DLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;
  else {  // NOLINT: Checking the if behavior.
    TAS_DLOG(1) << "Should NOT be executed";
  }

  if (true)
    TAS_DCHECK(true) << "An integer " << 1 << ", followed by a float " << 3.14;
  else
    TAS_DLOG(1) << "SHOULD be executed";

  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_DCHECK(false) << "An integer " << 1 << ", followed by a float " << 3.14;

#if TAS_HOST_TARGET
  LOG(INFO) << "LogExperimentTestFunction exit";
#endif
}

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_
