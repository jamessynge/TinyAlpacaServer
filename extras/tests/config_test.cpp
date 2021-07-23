#include "config.h"

#include "googletest/gtest.h"
#include "logging.h"
#include "utils/platform.h"
#include "utils/utils_config.h"

namespace alpaca {
namespace test {
namespace {

TEST(ConfigTest, LogInfo) {
#define LOG_MACRO(m) LOG(INFO) << #m << ": " << m;

  LOG_MACRO(TAS_EMBEDDED_TARGET);
  LOG_MACRO(TAS_HOST_TARGET);
  LOG_MACRO(TAS_ENABLE_DEBUGGING);
  LOG_MACRO(TAS_ENABLE_EXTRA_REQUEST_PARAMETERS);
  LOG_MACRO(TAS_MAX_EXTRA_REQUEST_PARAMETERS);
  LOG_MACRO(TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH);

#ifdef TAS_ENABLED_VLOG_LEVEL
  LOG_MACRO(TAS_ENABLED_VLOG_LEVEL);
#endif  // TAS_ENABLED_VLOG_LEVEL

#ifdef TAS_ENABLE_CHECK
  LOG(INFO) << "TAS_ENABLE_CHECK is defined";
#else   // !TAS_ENABLE_CHECK
  LOG(INFO) << "TAS_ENABLE_CHECK is NOT defined";
#endif  // TAS_ENABLE_CHECK

#ifdef TAS_ENABLE_DCHECK
  LOG(INFO) << "TAS_ENABLE_DCHECK is defined";
#else   // !TAS_ENABLE_DCHECK
  LOG(INFO) << "TAS_ENABLE_DCHECK is NOT defined";
#endif  // TAS_ENABLE_DCHECK
}

}  // namespace
}  // namespace test
}  // namespace alpaca
