#ifdef TAS_ENABLE_CHECK
#undef TAS_ENABLE_CHECK
#endif

#ifdef TAS_ENABLE_DCHECK
#undef TAS_ENABLE_DCHECK
#endif

#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif

#define TAS_ENABLE_LOG_EXPERIMENT

#include <TinyAlpacaServer.h>
