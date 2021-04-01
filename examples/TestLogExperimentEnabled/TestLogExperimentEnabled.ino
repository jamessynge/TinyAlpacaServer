#ifndef TAS_ENABLE_CHECK
#define TAS_ENABLE_CHECK
#endif

#ifndef TAS_ENABLE_DCHECK
#define TAS_ENABLE_DCHECK
#endif

#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif
#define TAS_ENABLED_VLOG_LEVEL 2

#define TAS_ENABLE_LOG_EXPERIMENT

#include <TinyAlpacaServer.h>
