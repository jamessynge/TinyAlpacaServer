#ifndef TINY_ALPACA_SERVER_SRC_UTILS_UTILS_CONFIG_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_UTILS_CONFIG_H_

// Controls the set of features available (enabled/disabled) in the utils of
// Tiny Alpaca Server. (This is in contrast to platform.h, which expresses the
// set of features provided by the platform.)
//
// This file should only define macros (and possibly constants), and not include
// (and hence export) anything else.
//
// Author: james.synge@gmail.com

#ifdef TAS_DO_LOG_EXPERIMENT
// Allow the file that is including LogExperiment.h to control the setting of
// the logging.h enable macros.

#ifdef TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH
namespace UTILS_CONFIG {
extern void [[TAS_DO_LOG_EXPERIMENT_is_defined]] UtilsFunc();
}
#endif  // TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

#else  // !TAS_DO_LOG_EXPERIMENT

#ifdef ARDUINO
// On Arduino we leave TAS_CHECK enabled, but not TAS_VLOG or TAS_DCHECK.

#ifndef TAS_ENABLED_VLOG_LEVEL
#define TAS_ENABLED_VLOG_LEVEL 3
#endif  //! TAS_ENABLED_VLOG_LEVEL

#ifndef TAS_ENABLE_CHECK
#define TAS_ENABLE_CHECK
#endif  // !TAS_ENABLE_CHECK

// #ifdef TAS_ENABLE_DCHECK
// #undef TAS_ENABLE_DCHECK
// #endif  // TAS_ENABLE_DCHECK

#ifndef TAS_ENABLE_DCHECK
#define TAS_ENABLE_DCHECK
#endif  // !TAS_ENABLE_DCHECK

#else  // !ARDUINO

#ifndef TAS_ENABLED_VLOG_LEVEL
#define TAS_ENABLED_VLOG_LEVEL 9
#endif  //! TAS_ENABLED_VLOG_LEVEL

#ifndef TAS_ENABLE_CHECK
#define TAS_ENABLE_CHECK
#endif  // !TAS_ENABLE_CHECK

#ifndef TAS_ENABLE_DCHECK
#ifndef NDEBUG
#define TAS_ENABLE_DCHECK
#endif  // !NDEBUG
#endif  // !TAS_ENABLE_DCHECK

#endif  // ARDUINO

// Last, we apply explicit disablement of the features. This allows a single .cc
// file to disable logging. For example, request_decoder has lots of logging,
// but it is now well tested, so doesn't need much logging. Note that this
// wouldn't be necessary if logging.h supported per-module conditional logging;
// for embedded, this would be best done at compile time, which is a tad tricky.

#ifdef TAS_DISABLE_VLOG
#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif  // TAS_ENABLED_VLOG_LEVEL
#define TAS_ENABLED_VLOG_LEVEL 0
#endif  // TAS_DISABLE_VLOG

#ifdef TAS_DISABLE_CHECK
#ifdef TAS_ENABLE_CHECK
#undef TAS_ENABLE_CHECK
#endif  // TAS_ENABLE_CHECK
#endif  // TAS_DISABLE_CHECK

#ifdef TAS_DISABLE_DCHECK
#ifdef TAS_ENABLE_DCHECK
#undef TAS_ENABLE_DCHECK
#endif  // TAS_ENABLE_DCHECK
#endif  // TAS_DISABLE_DCHECK

#endif  // TAS_DO_LOG_EXPERIMENT

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_UTILS_CONFIG_H_
