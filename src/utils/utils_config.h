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

// Allow the file that is including LogExperiment.h to control the setting of
// the logging.h enable macros.
#ifndef TAS_DO_LOG_EXPERIMENT

#ifdef ARDUINO
// On Arduino we leave TAS_CHECK enabled, but not TAS_VLOG or TAS_DCHECK.

#ifndef TAS_ENABLED_VLOG_LEVEL
#define TAS_ENABLED_VLOG_LEVEL 0
#endif  //! TAS_ENABLED_VLOG_LEVEL

#ifndef TAS_ENABLE_CHECK
#define TAS_ENABLE_CHECK
#endif  // !TAS_ENABLE_CHECK

#ifdef TAS_ENABLE_DCHECK
#undef TAS_ENABLE_DCHECK
#endif  // TAS_ENABLE_DCHECK

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

#endif  // !TAS_DO_LOG_EXPERIMENT

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_UTILS_CONFIG_H_
