/*
Tests of the flash and ram space required for Tiny Alpaca Server logging code when
disabled.

************************************************************************

TAS_ENABLE_INCLUDE_LOGGING_EXPERIMENT

Include logging, but no logging statements (should be same as the sketch
without anything included).

Sketch uses 2026 bytes (0%) of program storage space. Maximum is 253952 bytes.
Global variables use 220 bytes (2%) of dynamic memory, leaving 7972 bytes for local variables. Maximum is 8192 bytes.

************************************************************************

TAS_ENABLE_LOG_EXPERIMENT

Include logging and have some TAS_VLOG statements.

Sketch uses 2634 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 331 bytes (4%) of dynamic memory, leaving 7861 bytes for local variables. Maximum is 8192 bytes.


************************************************************************



*/



#ifdef TAS_ENABLE_CHECK
#undef TAS_ENABLE_CHECK
#endif

#ifdef TAS_ENABLE_DCHECK
#undef TAS_ENABLE_DCHECK
#endif

#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif

#define TAS_ENABLE_INCLUDE_LOGGING_EXPERIMENT

#include <LogExperiment.h>
