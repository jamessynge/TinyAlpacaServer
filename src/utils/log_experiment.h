#ifndef TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "utils/logging.h"

inline void LogExperimentTestFunction() {
#if TAS_HOST_TARGET
  LOG(INFO) << "LogExperimentTestFunction enter";
#endif

  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_VLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;

  if (true)
    TAS_VLOG(1) << "An integer " << 1 << ", followed by a float " << 3.14;
  else {  // NOLINT: Checking the if behavior.
    TAS_VLOG(1) << "Should NOT be executed";
  }

  if (true)
    TAS_CHECK(true) << "An integer " << 1 << ", followed by a float " << 3.14;
  else
    TAS_VLOG(1) << "SHOULD be executed";

  if (false) {
  } else  // NOLINT: Checking the else behavior.
    TAS_CHECK(false) << "An integer " << 1 << ", followed by a float " << 3.14;

#if TAS_HOST_TARGET
  LOG(INFO) << "LogExperimentTestFunction exit";
#endif
}

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_LOG_EXPERIMENT_H_
