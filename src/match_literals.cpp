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
bool MatchCoverCalibratorMethod(const StringView& view, EDeviceMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(brightness, EDeviceMethod::kBrightness);
  MATCH_ONE_LITERAL_EXACTLY(calibratoroff, EDeviceMethod::kCalibratorOff);
  MATCH_ONE_LITERAL_EXACTLY(calibratoron, EDeviceMethod::kCalibratorOn);
  MATCH_ONE_LITERAL_EXACTLY(calibratorstate, EDeviceMethod::kCalibratorState);
  MATCH_ONE_LITERAL_EXACTLY(closecover, EDeviceMethod::kCloseCover);
  MATCH_ONE_LITERAL_EXACTLY(coverstate, EDeviceMethod::kCoverState);
  MATCH_ONE_LITERAL_EXACTLY(haltcover, EDeviceMethod::kHaltCover);
  MATCH_ONE_LITERAL_EXACTLY(maxbrightness, EDeviceMethod::kMaxBrightness);
  MATCH_ONE_LITERAL_EXACTLY(opencover, EDeviceMethod::kOpenCover);
  return false;
}

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
  MATCH_ONE_LITERAL_EXACTLY(starfwhm, EDeviceMethod::kStarFWHM);
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

bool MatchSwitchMethod(const StringView& view, EDeviceMethod& match) {
  MATCH_ONE_LITERAL_EXACTLY(canwrite, EDeviceMethod::kCanWrite);
  MATCH_ONE_LITERAL_EXACTLY(getswitch, EDeviceMethod::kGetSwitch);
  MATCH_ONE_LITERAL_EXACTLY(getswitchdescription,
                            EDeviceMethod::kGetSwitchDescription);
  MATCH_ONE_LITERAL_EXACTLY(getswitchname, EDeviceMethod::kGetSwitchName);
  MATCH_ONE_LITERAL_EXACTLY(getswitchvalue, EDeviceMethod::kGetSwitchValue);
  MATCH_ONE_LITERAL_EXACTLY(maxswitch, EDeviceMethod::kMaxSwitch);
  MATCH_ONE_LITERAL_EXACTLY(maxswitchvalue, EDeviceMethod::kMaxSwitchValue);
  MATCH_ONE_LITERAL_EXACTLY(minswitchvalue, EDeviceMethod::kMinSwitchValue);
  MATCH_ONE_LITERAL_EXACTLY(setswitch, EDeviceMethod::kSetSwitch);
  MATCH_ONE_LITERAL_EXACTLY(setswitchname, EDeviceMethod::kSetSwitchName);
  MATCH_ONE_LITERAL_EXACTLY(setswitchvalue, EDeviceMethod::kSetSwitchValue);
  MATCH_ONE_LITERAL_EXACTLY(switchstep, EDeviceMethod::kSwitchStep);
  return false;
}

}  // namespace

bool MatchDeviceMethod(const EApiGroup group, const EDeviceType device_type,
                       const StringView& view, EDeviceMethod& match) {
  if (group == EApiGroup::kDevice) {
    switch (device_type) {
      case EDeviceType::kCoverCalibrator:
        if (MatchCoverCalibratorMethod(view, match)) {
          return true;
        }
        break;

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

      case EDeviceType::kSwitch:
        if (MatchSwitchMethod(view, match)) {
          return true;
        }
        break;

        //  ... cases for other device types ...

      default:
        // We don't automatically return an error right here because our goal is
        // is to return kHttpOk from the decoder if the request is well formed,
        // i.e. we don't want to decide too early that a kHttpBadRequest error
        // should be returned.
        TAS_VLOG(2) << device_type
                    << TAS_FLASHSTR(" is not supported by MatchDeviceMethod");
        break;
    }
    return internal::MatchCommonDeviceMethod(view, match);
  } else if (group == EApiGroup::kSetup) {
    // NOTE: Not checking whether the device type is supported.
    MATCH_ONE_LITERAL_EXACTLY(setup, EDeviceMethod::kSetup);
    return false;
  } else {
    TAS_CHECK(false) << TAS_FLASHSTR("api group (") << group
                     << TAS_FLASHSTR(") is not device or setup");
    return false;
  }
}

bool MatchParameter(const StringView& view, EParameter& match) {
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(action, EParameter::kAction);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(brightness, EParameter::kBrightness);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(ClientID, EParameter::kClientID);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(ClientTransactionID,
                                       EParameter::kClientTransactionID);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Command, EParameter::kCommand);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Connected, EParameter::kConnected);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Id, EParameter::kId);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Name, EParameter::kName);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Parameters, EParameter::kParameters);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Raw, EParameter::kRaw);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(SensorName, EParameter::kSensorName);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(State, EParameter::kState);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Value, EParameter::kValue);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(AveragePeriod,
                                       EParameter::kAveragePeriod);

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
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(starfwhm, ESensorName::kStarFWHM);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(temperature, ESensorName::kTemperature);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(winddirection,
                                       ESensorName::kWindDirection);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(windgust, ESensorName::kWindGust);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(windspeed, ESensorName::kWindSpeed);
  return false;
}

bool MatchHttpHeader(const StringView& view, EHttpHeader& match) {
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Connection, EHttpHeader::kConnection);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(HttpContentLength,
                                       EHttpHeader::kContentLength);
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(HttpContentType,
                                       EHttpHeader::kContentType);

  // Date is used in tests as an example of a header whose name we know but for
  // which there is not built-in decoding.
  MATCH_ONE_LITERAL_CASE_INSENSITIVELY(Date, EHttpHeader::kDate);
  return false;
}

}  // namespace alpaca
