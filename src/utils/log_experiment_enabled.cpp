#ifndef ARDUINO

#ifndef TAS_ENABLE_CHECK
#define TAS_ENABLE_CHECK
#endif

#ifdef TAS_ENABLED_VLOG_LEVEL
#undef TAS_ENABLED_VLOG_LEVEL
#endif
#define TAS_ENABLED_VLOG_LEVEL 2

#include "base/init_google.h"
#include "utils/log_experiment.h"

int main(int argc, char* argv[]) {
  InitGoogle(argv[0], &argc, &argv, /*remove_flags=*/true);
  LogExperimentTestFunction();
  return 0;
}

#endif
