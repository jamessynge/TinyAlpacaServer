#include "match_literals.h"

#include "constants.h"
#include "literals.h"
#include "utils/array_view.h"
#include "utils/literal.h"
#include "utils/logging.h"
#include "utils/string_compare.h"

#define MATCH_ONE_LITERAL_EXACTLY(literal_name, enum_value) \
  if (Literals::literal_name() == view) {                   \
    match = enum_value;                                     \
    return true;                                            \
  }

#define MATCH_ONE_LITERAL_CASE_INSENSITIVELY(literal_name, enum_value) \
  if (CaseEqual(Literals::literal_name(), view)) {                     \
    match = enum_value;                                                \
    return true;                                                       \
  }

namespace alpaca {

bool MatchHttpMethod(const StringView& view, EHttpMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(GET, EHttpMethod::GET);
  MATCH_ONE_LITERAL_EXACTLY(PUT, EHttpMethod::PUT);
  MATCH_ONE_LITERAL_EXACTLY(HEAD, EHttpMethod::HEAD);
  return false;
}

bool MatchApiGroup(const StringView& view, EApiGroup& match) {
  MATCH_ONE_LITERAL_EXACTLY(api, EApiGroup::kDevice);
  MATCH_ONE_LITERAL_EXACTLY(management, EApiGroup::kManagement);
  MATCH_ONE_LITERAL_EXACTLY(setup, EApiGroup::kSetup);
  return false;
}

bool MatchManagementMethod(const StringView& view, EManagementMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(description, EManagementMethod::kDescription);
  MATCH_ONE_LITERAL_EXACTLY(configureddevices,
                            EManagementMethod::kConfiguredDevices);
  return false;
}

bool MatchDeviceType(const StringView& view, EDeviceType& match) {
  MATCH_ONE_LITERAL_EXACTLY(camera, EDeviceType::kCamera);
  MATCH_ONE_LITERAL_EXACTLY(covercalibrator, EDeviceType::kCoverCalibrator);
  MATCH_ONE_LITERAL_EXACTLY(dome, EDeviceType::kDome);
  MATCH_ONE_LITERAL_EXACTLY(filterwheel, EDeviceType::kFilterWheel);
  MATCH_ONE_LITERAL_EXACTLY(focuser, EDeviceType::kFocuser);
  MATCH_ONE_LITERAL_EXACTLY(observingconditions,
                            EDeviceType::kObservingConditions);
  MATCH_ONE_LITERAL_EXACTLY(rotator, EDeviceType::kRotator);
  MATCH_ONE_LITERAL_EXACTLY(safetymonitor, EDeviceType::kSafetyMonitor);
  MATCH_ONE_LITERAL_EXACTLY(DeviceTypeSwitch, EDeviceType::kSwitch);
  MATCH_ONE_LITERAL_EXACTLY(telescope, EDeviceType::kTelescope);
  return false;
}

namespace internal {
// Exposed for testing.
bool MatchCommonDeviceMethod(const StringView& view, EDeviceMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(connected, EDeviceMethod::kConnected);
  MATCH_ONE_LITERAL_EXACTLY(description, EDeviceMethod::kDescription);
  MATCH_ONE_LITERAL_EXACTLY(driverinfo, EDeviceMethod::kDriverInfo);
  MATCH_ONE_LITERAL_EXACTLY(driverversion, EDeviceMethod::kDriverVersion);
  MATCH_ONE_LITERAL_EXACTLY(interfaceversion, EDeviceMethod::kInterfaceVersion);
  MATCH_ONE_LITERAL_EXACTLY(name, EDeviceMethod::kName);
  MATCH_ONE_LITERAL_EXACTLY(supportedactions, EDeviceMethod::kSupportedActions);
  return false;
}
}  // namespace internal

namespace {
bool MatchObservingConditionsMethod(const StringView& view,
                                    EDeviceMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(averageperiod, EDeviceMethod::kAveragePeriod);
  MATCH_ONE_LITERAL_EXACTLY(cloudcover, EDeviceMethod::kCloudCover);
  MATCH_ONE_LITERAL_EXACTLY(dewpoint, EDeviceMethod::kDewPoint);
  MATCH_ONE_LITERAL_EXACTLY(humidity, EDeviceMethod::kHumidity);
  MATCH_ONE_LITERAL_EXACTLY(pressure, EDeviceMethod::kPressure);
  MATCH_ONE_LITERAL_EXACTLY(rainrate, EDeviceMethod::kRainRate);
  MATCH_ONE_LITERAL_EXACTLY(refresh, EDeviceMethod::kRefresh);
  MATCH_ONE_LITERAL_EXACTLY(sensordescription,
                            EDeviceMethod::kSensorDescription);
  MATCH_ONE_LITERAL_EXACTLY(skybrightness, EDeviceMethod::kSkyBrightness);
  MATCH_ONE_LITERAL_EXACTLY(skyquality, EDeviceMethod::kSkyQuality);
  MATCH_ONE_LITERAL_EXACTLY(skytemperature, EDeviceMethod::kSkyTemperature);
  MATCH_ONE_LITERAL_EXACTLY(starfullwidthhalfmax,
                            EDeviceMethod::kStarFullWidthHalfMax);
  MATCH_ONE_LITERAL_EXACTLY(temperature, EDeviceMethod::kTemperature);
  MATCH_ONE_LITERAL_EXACTLY(timesincelastupdate,
                            EDeviceMethod::kTimeSinceLastUpdate);
  MATCH_ONE_LITERAL_EXACTLY(winddirection, EDeviceMethod::kWindDirection);
  MATCH_ONE_LITERAL_EXACTLY(windgust, EDeviceMethod::kWindGust);
  MATCH_ONE_LITERAL_EXACTLY(windspeed, EDeviceMethod::kWindSpeed);
  return false;
}

bool MatchSafetyMonitorMethod(const StringView& view, EDeviceMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(issafe, EDeviceMethod::kIsSafe);
  return false;
}
}  // namespace

bool MatchDeviceMethod(const EApiGroup group, const EDeviceType device_type,
                       const StringView& view, EDeviceMethod& match) {
  if (group == EApiGroup::kDevice) {
    switch (device_type) {
      case EDeviceType::kObservingConditions:
        if (MatchObservingConditionsMethod(view, match)) {
          return true;
        }
        break;

      case EDeviceType::kSafetyMonitor:
        if (MatchSafetyMonitorMethod(view, match)) {
          return true;
        }
        break;

        //  ... cases for other device types ...

      default:
        // We don't automatically return an error right here because our goal
        // is to return kHttpOk from the decoder if the request is well
        // formed, i.e. we don't want to decide too early that a kHttpNotFound
        // error should be returned.
        TAS_VLOG(2) << TASLIT("device type (") << device_type
                    << TASLIT(") is not supported");
        break;
    }
    return internal::MatchCommonDeviceMethod(view, match);
  } else if (group == EApiGroup::kSetup) {
    // NOTE: Not checking whether the device type is supported.
    MATCH_ONE_LITERAL_EXACTLY(setup, EDeviceMethod::kSetup);
    return false;
  } else {
    TAS_CHECK(false) << TASLIT("api group (") << group
                     << TASLIT(") is not device or setup");
    return false;
  }
}

bool MatchParameter(const StringView& view, EParameter& match) {
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(ClientId, EParameter::kClientId);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(ClientTransactionId,
                                       EParameter::kClientTransactionId);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Connected, EParameter::kConnected);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(SensorName, EParameter::kSensorName);
  return false;
}

bool MatchSensorName(const StringView& view, ESensorName& match) {
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(cloudcover, ESensorName::kCloudCover);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(dewpoint, ESensorName::kDewPoint);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(humidity, ESensorName::kHumidity);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(pressure, ESensorName::kPressure);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(rainrate, ESensorName::kRainRate);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(skybrightness,
                                       ESensorName::kSkyBrightness);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(skyquality, ESensorName::kSkyQuality);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(skytemperature,
                                       ESensorName::kSkyTemperature);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(starfullwidthhalfmax,
                                       ESensorName::kStarFullWidthHalfMax);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(temperature, ESensorName::kTemperature);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(winddirection,
                                       ESensorName::kWindDirection);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(windgust, ESensorName::kWindGust);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(windspeed, ESensorName::kWindSpeed);
  return false;
}

bool MatchHttpHeader(const StringView& view, EHttpHeader& match) {
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(HttpAccept, EHttpHeader::kHttpAccept);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(HttpContentLength,
                                       EHttpHeader::kHttpContentLength);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(HttpContentType,
                                       EHttpHeader::kHttpContentType);

  // Content-Encoding is used in tests as an example of a header we know
  // the name of, but don't have built-in handling for the value. It isn't
  // clear whether this is generally useful.
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(HttpContentEncoding,
                                       EHttpHeader::kHttpContentEncoding);
  return false;
}

}  // namespace alpaca
