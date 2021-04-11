/*
Tests of the flash and ram space required for Tiny Alpaca Server logging code
when disabled.

************************************************************************

TAS_LOG_EXPERIMENT_DO_INCLUDE_LOGGING_H

Include logging.h, but no logging statements (should be same as the sketch
without anything included).


************************************************************************

TAS_LOG_EXPERIMENT_DO_LOG

Include logging and have some TAS_VLOG statements.



************************************************************************

*/

// Make setup and loop available.

#define TAS_DO_LOG_EXPERIMENT

// Ask compiler to emit warnings indicating which branch of various
// #if/#else/#endif blocks have been taken.

#define TAS_LOG_EXPERIMENT_DO_ANNOUNCE_BRANCH

// Enable testing of specific features.

#define TAS_LOG_EXPERIMENT_DO_INCLUDE_LOGGING_H
#define TAS_LOG_EXPERIMENT_DO_LOG
#define TAS_LOG_EXPERIMENT_DO_CHECK
#define TAS_LOG_EXPERIMENT_DO_DCHECK

// Enable the requisite features of logging.h.
#ifdef TAS_LOG_EXPERIMENT_DO_LOG
#define TAS_ENABLED_VLOG_LEVEL 2
#endif

#ifdef TAS_LOG_EXPERIMENT_DO_CHECK
#define TAS_ENABLE_CHECK
#endif

#ifdef TAS_LOG_EXPERIMENT_DO_DCHECK
#define TAS_ENABLE_DCHECK
#endif

#include "LogExperiment.h"
