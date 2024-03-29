#include "match_literals.h"

#include <McuCore.h>

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/log.h"
#include "absl/strings/ascii.h"
#include "absl/strings/escaping.h"
#include "constants.h"
#include "gtest/gtest.h"
#include "mcucore/extras/test_tools/string_view_utils.h"

namespace alpaca {
namespace test {
namespace {

using ::mcucore::StringView;
using ::mcucore::test::MakeStringView;

TEST(MatchLiteralsTest, EHttpMethod) {
  const std::vector<std::pair<std::string, EHttpMethod>> test_cases = {
      {"GET", EHttpMethod::GET},       {"PUT", EHttpMethod::PUT},
      {"HEAD", EHttpMethod::HEAD},     {"get", EHttpMethod::kUnknown},
      {"POST", EHttpMethod::kUnknown}, {"", EHttpMethod::kUnknown},
  };
  const EHttpMethod kBogusEnum = static_cast<EHttpMethod>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EHttpMethod matched = kBogusEnum;
    if (expected_enum == EHttpMethod::kUnknown) {
      EXPECT_FALSE(MatchHttpMethod(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchHttpMethod(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

TEST(MatchLiteralsTest, EApiGroup) {
  const std::vector<std::pair<std::string, EApiGroup>> test_cases = {
      {"api", EApiGroup::kDevice},
      {"management", EApiGroup::kManagement},
      {"asset", EApiGroup::kAsset},
      {"setup", EApiGroup::kSetup},
      {"API", EApiGroup::kUnknown},
      {"Asset", EApiGroup::kUnknown},
      {"managements", EApiGroup::kUnknown},
      {"SetUp", EApiGroup::kUnknown},
      {"assets", EApiGroup::kUnknown},
      {"", EApiGroup::kUnknown},
  };
  const EApiGroup kBogusEnum = static_cast<EApiGroup>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EApiGroup matched = kBogusEnum;
    if (expected_enum == EApiGroup::kUnknown) {
      EXPECT_FALSE(MatchApiGroup(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchApiGroup(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

TEST(MatchLiteralsTest, MatchManagementMethod) {
  const std::vector<std::pair<std::string, EManagementMethod>> test_cases = {
      {"description", EManagementMethod::kDescription},
      {"configureddevices", EManagementMethod::kConfiguredDevices},
      {"setup", EManagementMethod::kUnknown},
      {"api", EManagementMethod::kUnknown},
      {"Description", EManagementMethod::kUnknown},
      {"Configureddevices", EManagementMethod::kUnknown},
      {"", EManagementMethod::kUnknown},
  };
  const EManagementMethod kBogusEnum = static_cast<EManagementMethod>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EManagementMethod matched = kBogusEnum;
    if (expected_enum == EManagementMethod::kUnknown) {
      EXPECT_FALSE(MatchManagementMethod(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchManagementMethod(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

TEST(MatchLiteralsTest, MatchDeviceType) {
  const std::vector<std::pair<std::string, EDeviceType>> test_cases = {
      {"camera", EDeviceType::kCamera},
      {"covercalibrator", EDeviceType::kCoverCalibrator},
      {"dome", EDeviceType::kDome},
      {"filterwheel", EDeviceType::kFilterWheel},
      {"focuser", EDeviceType::kFocuser},
      {"observingconditions", EDeviceType::kObservingConditions},
      {"rotator", EDeviceType::kRotator},
      {"safetymonitor", EDeviceType::kSafetyMonitor},
      {"switch", EDeviceType::kSwitch},
      {"telescope", EDeviceType::kTelescope},
      {"", EDeviceType::kUnknown},
      {"paramount", EDeviceType::kUnknown},
  };
  const EDeviceType kBogusEnum = static_cast<EDeviceType>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EDeviceType matched = kBogusEnum;
    if (expected_enum == EDeviceType::kUnknown) {
      EXPECT_FALSE(MatchDeviceType(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchDeviceType(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

using DeviceMethodTestCases =
    std::vector<std::pair<std::string, EDeviceMethod>>;

void PrependCommonDeviceMethodTestCases(DeviceMethodTestCases& test_cases) {
  const DeviceMethodTestCases kCommonDeviceMethods = {
      {"action", EDeviceMethod::kAction},
      {"commandblind", EDeviceMethod::kCommandBlind},
      {"commandbool", EDeviceMethod::kCommandBool},
      {"commandstring", EDeviceMethod::kCommandString},
      {"connected", EDeviceMethod::kConnected},
      {"description", EDeviceMethod::kDescription},
      {"driverinfo", EDeviceMethod::kDriverInfo},
      {"driverversion", EDeviceMethod::kDriverVersion},
      {"interfaceversion", EDeviceMethod::kInterfaceVersion},
      {"name", EDeviceMethod::kName},
      {"supportedactions", EDeviceMethod::kSupportedActions},
  };

  test_cases.insert(test_cases.cbegin(), kCommonDeviceMethods.begin(),
                    kCommonDeviceMethods.end());
}

TEST(MatchLiteralsTest, MatchCommonDeviceMethod) {
  DeviceMethodTestCases test_cases = {
      {"", EDeviceMethod::kUnknown},
      {"findabluemoon", EDeviceMethod::kUnknown},
      {"issafe", EDeviceMethod::kUnknown},
      {"setup", EDeviceMethod::kUnknown},
  };
  PrependCommonDeviceMethodTestCases(test_cases);

  const EDeviceMethod kBogusEnum = static_cast<EDeviceMethod>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EDeviceMethod matched = kBogusEnum;
    if (expected_enum == EDeviceMethod::kUnknown) {
      EXPECT_FALSE(
          internal::MatchCommonDeviceMethod(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(
          internal::MatchCommonDeviceMethod(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

TEST(MatchLiteralsTest, MatchCoverCalibratorMethod) {
  const EDeviceMethod kBogusEnum = static_cast<EDeviceMethod>(0xff);
  const EDeviceType kDeviceType = EDeviceType::kCoverCalibrator;

  // Tests of the Device API, as opposed to the Setup API.
  DeviceMethodTestCases test_cases = {
      {"brightness", EDeviceMethod::kBrightness},
      {"calibratoroff", EDeviceMethod::kCalibratorOff},
      {"calibratoron", EDeviceMethod::kCalibratorOn},
      {"calibratorstate", EDeviceMethod::kCalibratorState},
      {"closecover", EDeviceMethod::kCloseCover},
      {"coverstate", EDeviceMethod::kCoverState},
      {"haltcover", EDeviceMethod::kHaltCover},
      {"maxbrightness", EDeviceMethod::kMaxBrightness},
      {"opencover", EDeviceMethod::kOpenCover},
      {"", EDeviceMethod::kUnknown},
      {"cloudcover", EDeviceMethod::kUnknown},
      {"setup", EDeviceMethod::kUnknown},
  };
  PrependCommonDeviceMethodTestCases(test_cases);

  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EDeviceMethod matched = kBogusEnum;
    if (expected_enum == EDeviceMethod::kUnknown) {
      EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                     MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                    MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }

  // Now tests of the Setup API.
  EDeviceMethod matched = kBogusEnum;
  EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                 StringView("connected"), matched));
  EXPECT_EQ(matched, kBogusEnum);
  EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                StringView("setup"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kSetup);
}

TEST(MatchLiteralsTest, MatchObservingConditionsMethod) {
  const EDeviceMethod kBogusEnum = static_cast<EDeviceMethod>(0xff);
  const EDeviceType kDeviceType = EDeviceType::kObservingConditions;

  // Tests of the Device API, as opposed to the Setup API.
  DeviceMethodTestCases test_cases = {
      {"averageperiod", EDeviceMethod::kAveragePeriod},
      {"cloudcover", EDeviceMethod::kCloudCover},
      {"dewpoint", EDeviceMethod::kDewPoint},
      {"humidity", EDeviceMethod::kHumidity},
      {"pressure", EDeviceMethod::kPressure},
      {"rainrate", EDeviceMethod::kRainRate},
      {"refresh", EDeviceMethod::kRefresh},
      {"sensordescription", EDeviceMethod::kSensorDescription},
      {"temperature", EDeviceMethod::kTemperature},
      {"", EDeviceMethod::kUnknown},
      {"issafe", EDeviceMethod::kUnknown},
      {"setup", EDeviceMethod::kUnknown},
  };
  PrependCommonDeviceMethodTestCases(test_cases);

  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EDeviceMethod matched = kBogusEnum;
    if (expected_enum == EDeviceMethod::kUnknown) {
      EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                     MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                    MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }

  // Now tests of the Setup API.
  EDeviceMethod matched = kBogusEnum;
  EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                 StringView("connected"), matched));
  EXPECT_EQ(matched, kBogusEnum);
  EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                StringView("setup"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kSetup);
}

TEST(MatchLiteralsTest, MatchSafetyMonitorMethod) {
  const EDeviceMethod kBogusEnum = static_cast<EDeviceMethod>(0xff);
  const EDeviceType kDeviceType = EDeviceType::kSafetyMonitor;

  // Tests of the Device API, as opposed to the Setup API.
  DeviceMethodTestCases test_cases = {
      {"issafe", EDeviceMethod::kIsSafe},
      {"", EDeviceMethod::kUnknown},
      {"cloudcover", EDeviceMethod::kUnknown},
      {"setup", EDeviceMethod::kUnknown},
  };
  PrependCommonDeviceMethodTestCases(test_cases);

  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EDeviceMethod matched = kBogusEnum;
    if (expected_enum == EDeviceMethod::kUnknown) {
      EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                     MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                    MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }

  // Now tests of the Setup API.
  EDeviceMethod matched = kBogusEnum;
  EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                 StringView("connected"), matched));
  EXPECT_EQ(matched, kBogusEnum);
  EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                StringView("setup"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kSetup);
}

TEST(MatchLiteralsTest, MatchSwitchMethod) {
  const EDeviceMethod kBogusEnum = static_cast<EDeviceMethod>(0xff);
  const EDeviceType kDeviceType = EDeviceType::kSwitch;

  // Tests of the Device API, as opposed to the Setup API.
  DeviceMethodTestCases test_cases = {
      {"canwrite", EDeviceMethod::kCanWrite},
      {"getswitch", EDeviceMethod::kGetSwitch},
      {"getswitchdescription", EDeviceMethod::kGetSwitchDescription},
      {"getswitchname", EDeviceMethod::kGetSwitchName},
      {"getswitchvalue", EDeviceMethod::kGetSwitchValue},
      {"maxswitch", EDeviceMethod::kMaxSwitch},
      {"maxswitchvalue", EDeviceMethod::kMaxSwitchValue},
      {"minswitchvalue", EDeviceMethod::kMinSwitchValue},
      {"setswitchvalue", EDeviceMethod::kSetSwitchValue},
      {"switchstep", EDeviceMethod::kSwitchStep},
      {"", EDeviceMethod::kUnknown},
      {"issafe", EDeviceMethod::kUnknown},
      {"averageperiod", EDeviceMethod::kUnknown},
      {"setup", EDeviceMethod::kUnknown},
  };
  PrependCommonDeviceMethodTestCases(test_cases);

  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EDeviceMethod matched = kBogusEnum;
    if (expected_enum == EDeviceMethod::kUnknown) {
      EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                     MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kDevice, kDeviceType,
                                    MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }

  // Now tests of the Setup API.
  EDeviceMethod matched = kBogusEnum;
  EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                 StringView("connected"), matched));
  EXPECT_EQ(matched, kBogusEnum);
  EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kSetup, kDeviceType,
                                StringView("setup"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kSetup);
}

TEST(MatchLiteralsTest, MatchDeviceMethodUnsupportedDeviceType) {
  // Using kUnknown so this test doesn't break in the future if support is added
  // for more device types.
  EDeviceMethod matched = EDeviceMethod::kUnknown;

  EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kDevice, EDeviceType::kUnknown,
                                 StringView("issafe"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kUnknown);

  EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kDevice, EDeviceType::kUnknown,
                                 StringView("setup"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kUnknown);

  EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kDevice, EDeviceType::kUnknown,
                                StringView("name"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kName);

  EXPECT_TRUE(MatchDeviceMethod(EApiGroup::kSetup, EDeviceType::kUnknown,
                                StringView("setup"), matched));
  EXPECT_EQ(matched, EDeviceMethod::kSetup);
}

TEST(MatchLiteralsDeathTest, MatchDeviceMethodWrongApiGroup) {
  EXPECT_DEBUG_DEATH(
      {
        const EDeviceMethod kBogusEnum = static_cast<EDeviceMethod>(0xff);
        EDeviceMethod matched = kBogusEnum;
        EXPECT_FALSE(MatchDeviceMethod(EApiGroup::kManagement,
                                       EDeviceType::kSafetyMonitor,
                                       StringView("name"), matched));
        EXPECT_EQ(matched, kBogusEnum);
      },
      "api group \\(Management\\) is not device or setup");
}

TEST(MatchLiteralsTest, MatchParameter) {
  const std::vector<std::pair<std::string, EParameter>> test_cases = {
      {"Action", EParameter::kAction},
      {"AveragePeriod", EParameter::kAveragePeriod},
      {"Brightness", EParameter::kBrightness},
      {"ClientId", EParameter::kClientID},
      {"ClientTransactionId", EParameter::kClientTransactionID},
      {"Command", EParameter::kCommand},
      {"Connected", EParameter::kConnected},
      {"Id", EParameter::kId},
      {"Name", EParameter::kName},
      {"Parameters", EParameter::kParameters},
      {"Raw", EParameter::kRaw},
      {"SensorName", EParameter::kSensorName},
      {"State", EParameter::kState},
      {"Value", EParameter::kValue},
      {"", EParameter::kUnknown},
      {"cloudcover", EParameter::kUnknown},
      {"Content-Length", EParameter::kUnknown},
  };
  // Matching should be case insensitive, so we transform the string to lower
  // and upper after checking that it works in the original case.
  std::vector<std::function<std::string(std::string)>> string_transformer = {
      [](std::string s) -> std::string { return s; },
      [](std::string s) -> std::string { return absl::AsciiStrToLower(s); },
      [](std::string s) -> std::string { return absl::AsciiStrToUpper(s); },
  };

  const EParameter kBogusEnum = static_cast<EParameter>(0xff);
  for (const auto [test_case_text, expected_enum] : test_cases) {
    for (const auto transformer_fn : string_transformer) {
      std::string text = transformer_fn(test_case_text);
      VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
              << expected_enum;
      EParameter matched = kBogusEnum;
      if (expected_enum == EParameter::kUnknown) {
        EXPECT_FALSE(MatchParameter(MakeStringView(text), matched));
        EXPECT_EQ(matched, kBogusEnum);
      } else {
        EXPECT_TRUE(MatchParameter(MakeStringView(text), matched));
        EXPECT_EQ(matched, expected_enum);
      }
    }
  }
}

TEST(MatchLiteralsTest, MatchSensorName) {
  const std::vector<std::pair<std::string, ESensorName>> test_cases = {
      {"CloudCover", ESensorName::kCloudCover},
      {"DEWPOINT", ESensorName::kDewPoint},
      {"huMIDity", ESensorName::kHumidity},
      {"pressure", ESensorName::kPressure},
      {"RAINrate", ESensorName::kRainRate},
      {"skybrightness", ESensorName::kSkyBrightness},
      {"skyQuality", ESensorName::kSkyQuality},
      {"", ESensorName::kUnknown},
      {"cloud-cover", ESensorName::kUnknown},
      {"Content-Length", ESensorName::kUnknown},
  };
  const ESensorName kBogusEnum = static_cast<ESensorName>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    ESensorName matched = kBogusEnum;
    if (expected_enum == ESensorName::kUnknown) {
      EXPECT_FALSE(MatchSensorName(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchSensorName(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

TEST(MatchLiteralsTest, EHttpHeader) {
  const std::vector<std::pair<std::string, EHttpHeader>> test_cases = {
      {"Content-LENGTH", EHttpHeader::kContentLength},
      {"content-type", EHttpHeader::kContentType},
      {"Date", EHttpHeader::kDate},
      {"", EHttpHeader::kUnknown},
      {"Accept", EHttpHeader::kUnknown},
  };
  const EHttpHeader kBogusEnum = static_cast<EHttpHeader>(0xff);
  for (const auto [text, expected_enum] : test_cases) {
    VLOG(1) << "Matching '" << absl::CHexEscape(text) << "', expecting "
            << expected_enum;
    EHttpHeader matched = kBogusEnum;
    if (expected_enum == EHttpHeader::kUnknown) {
      EXPECT_FALSE(MatchHttpHeader(MakeStringView(text), matched));
      EXPECT_EQ(matched, kBogusEnum);
    } else {
      EXPECT_TRUE(MatchHttpHeader(MakeStringView(text), matched));
      EXPECT_EQ(matched, expected_enum);
    }
  }
}

}  // namespace
}  // namespace test
}  // namespace alpaca
