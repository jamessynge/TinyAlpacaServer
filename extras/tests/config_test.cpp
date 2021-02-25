#include "config.h"

#include "googletest/gmock.h"
#include "googletest/gtest.h"
#include "platform.h"

namespace {

TEST(ConfigTest, LogInfo) {
  LOG(INFO) << "\n\n"
            << "TAS_EMBEDDED_TARGET: " << TAS_EMBEDDED_TARGET << "\n"
            << "TAS_HOST_TARGET: " << TAS_HOST_TARGET << "\n"  // Line break
            << "TAS_ENABLE_DEBUGGING: " << TAS_ENABLE_DEBUGGING << "\n"
            << "TAS_ENABLE_EXTRA_REQUEST_PARAMETERS: "
            << TAS_ENABLE_EXTRA_REQUEST_PARAMETERS << "\n"
            << "TAS_MAX_EXTRA_REQUEST_PARAMETERS: "
            << TAS_MAX_EXTRA_REQUEST_PARAMETERS << "\n"
            << "TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH: "
            << TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH << "\n";
}

}  // namespace
