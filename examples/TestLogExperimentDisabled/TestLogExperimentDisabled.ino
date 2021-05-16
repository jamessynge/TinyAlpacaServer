/*
Tests of the flash and ram space required for Tiny Alpaca Server logging code
when disabled.

************************************************************************

TAS_DO_LOG_EXPERIMENT

Just a basic setup & loop with Serial.begin, Serial operator bool,
Serial.print(char), Serial.println(char)

Sketch uses 2594 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 327 bytes (3%) of dynamic memory,
leaving 7865 bytes for local variables. Maximum is 8192 bytes.

This is far too large, which means that somewhere in TAS or its dependencies
we're picking up some code and/or data that shouldn't be included in the
linked binary.

************************************************************************

TAS_LOG_EXPERIMENT_DO_INCLUDE_LOGGING_H

Include logging.h, platform.h and inline_literal.h.

Sketch uses 2594 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 327 bytes (3%) of dynamic memory,
leaving 7865 bytes for local variables. Maximum is 8192 bytes.

At least we didn't pickup any additional stuff.

************************************************************************

TAS_LOG_EXPERIMENT_DO_LOG

Several TAS_VLOG statements, but TAS_ENABLED_VLOG_LEVEL is undefined.

Sketch uses 2594 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 327 bytes (3%) of dynamic memory, leaving 7865 bytes for
local variables. Maximum is 8192 bytes.

At least we didn't pickup any additional stuff.

************************************************************************

TAS_LOG_EXPERIMENT_DO_CHECK

Several TAS_CHECK statements, but TAS_ENABLE_CHECK is undefined.

Sketch uses 2594 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 327 bytes (3%) of dynamic memory, leaving 7865 bytes for
local variables. Maximum is 8192 bytes.

At least we didn't pickup any additional stuff.

************************************************************************

TAS_LOG_EXPERIMENT_DO_DCHECK

Several TAS_DCHECK statements, but TAS_ENABLE_DCHECK is undefined.

Sketch uses 2594 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 327 bytes (3%) of dynamic memory, leaving 7865 bytes for
local variables. Maximum is 8192 bytes.

At least we didn't pickup any additional stuff. The same applies if
TAS_ENABLE_CHECK is defined while TAS_ENABLE_DCHECK is undefined.

*/

// Make setup and loop available.

#define TAS_DO_LOG_EXPERIMENT

// Enable testing of specific features.

//#define TAS_LOG_EXPERIMENT_DO_INCLUDE_LOGGING_H
//#define TAS_LOG_EXPERIMENT_DO_LOG
//#define TAS_LOG_EXPERIMENT_DO_CHECK
#define TAS_LOG_EXPERIMENT_DO_DCHECK

// Disable all logging.

#ifdef TAS_ENABLE_CHECK
#undef TAS_ENABLE_CHECK
#endif

#ifdef TAS_ENABLE_DCHECK
#undef TAS_ENABLE_DCHECK
#endif

#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif

#include <LogExperiment.h>
