#include "match_literals.h"

// NOTE: This uses reinterpret_cast, which is not safe. Why? I'm aiming to avoid
// excessive code size due to template expansion, so I'd normally use
// std::underlying_type<EnumTypeName>, but on the Arduino we don't have
// <type_traits>.
//

#include "constants.h"
#include "literals.h"
#include "utils/array_view.h"
#include "utils/literal.h"
#include "utils/literal_token.h"
#include "utils/logging.h"

// WARNING: Arrays built using MAKE_ENUM_LITERAL_TOKEN take up a bunch of RAM;
// i.e. on AVR chips they get copied from flash (aka PROGMEM) to RAM at boot.
// They probably take 3 or 5 bytes per entry currently:
//
// * 2 or 4 bytes for a pointer to a function in flash;
// * 1 byte for an enum value.
//
// I'm not sure if alignment is an issue; if it is, then it probably rounds up
// so that the array element size is 4 or 8 bytes.
//
// An alternative (explored below) is to explicitly compare the string against
// each literal. We'll see if that saves a bunch of memory.

#define MAKE_ENUM_LITERAL_TOKEN(enum_type, literal_name, enum_value)   \
  LiteralToken<enum_type##_UnderlyingType> {                           \
    Literals::literal_name,                                            \
        static_cast<enum_type##_UnderlyingType>(enum_type::enum_value) \
  }

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

#if 0

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EHttpMethod_UnderlyingType>
    kRecognizedHttpMethods[] = {
        MAKE_ENUM_LITERAL_TOKEN(EHttpMethod, GET, GET),
        MAKE_ENUM_LITERAL_TOKEN(EHttpMethod, PUT, PUT),
        MAKE_ENUM_LITERAL_TOKEN(EHttpMethod, HEAD, HEAD),
};
}  // namespace

bool MatchHttpMethod(const StringView& view, EHttpMethod& match) {
  auto* match_ptr = reinterpret_cast<EHttpMethod_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(
      view, MakeArrayView(kRecognizedHttpMethods), *match_ptr);
}

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EApiGroup_UnderlyingType> kAllApiGroups[] = {
    MAKE_ENUM_LITERAL_TOKEN(EApiGroup, api, kDevice),
    MAKE_ENUM_LITERAL_TOKEN(EApiGroup, management, kManagement),
    MAKE_ENUM_LITERAL_TOKEN(EApiGroup, setup, kSetup),
};
}  // namespace

bool MatchApiGroup(const StringView& view, EApiGroup& match) {
  auto* match_ptr = reinterpret_cast<EApiGroup_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(view, MakeArrayView(kAllApiGroups),
                                        *match_ptr);
}

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EManagementMethod_UnderlyingType>
    kAllManagementMethods[] = {
        // Case sensitive comparison, these strings must be LOWERCASE.
        MAKE_ENUM_LITERAL_TOKEN(EManagementMethod, description, kDescription),
        MAKE_ENUM_LITERAL_TOKEN(EManagementMethod, configureddevices,
                                kConfiguredDevices),
};
}  // namespace

bool MatchManagementMethod(const StringView& view, EManagementMethod& match) {
  auto* match_ptr = reinterpret_cast<EManagementMethod_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(
      view, MakeArrayView(kAllManagementMethods), *match_ptr);
}

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EDeviceType_UnderlyingType>
    kRecognizedDeviceTypes[] = {
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, camera, kCamera),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, covercalibrator, kCoverCalibrator),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, dome, kDome),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, filterwheel, kFilterWheel),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, focuser, kFocuser),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, observingconditions,
                                kObservingConditions),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, rotator, kRotator),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, safetymonitor, kSafetyMonitor),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, DeviceTypeSwitch, kSwitch),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceType, telescope, kTelescope),
};
}  // namespace

bool MatchDeviceType(const StringView& view, EDeviceType& match) {
  auto* match_ptr = reinterpret_cast<EDeviceType_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(
      view, MakeArrayView(kRecognizedDeviceTypes), *match_ptr);
}

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EDeviceMethod_UnderlyingType>
    kCommonDeviceMethods[] = {
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, connected, kConnected),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, description, kDescription),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, driverinfo, kDriverInfo),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, driverversion, kDriverVersion),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, interfaceversion,
                                kInterfaceVersion),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, name, kName),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, supportedactions,
                                kSupportedActions),
};
}  // namespace

namespace internal {
// Exposed for testing.
bool MatchCommonDeviceMethod(const StringView& view, EDeviceMethod& match) {
  auto* match_ptr = reinterpret_cast<EDeviceMethod_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(
      view, MakeArrayView(kCommonDeviceMethods), *match_ptr);
}
}  // namespace internal

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EDeviceMethod_UnderlyingType>
    kObservingConditionsMethods[] = {
        // Case sensitive comparison, these strings must be LOWERCASE.
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, averageperiod, kAveragePeriod),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, cloudcover, kCloudCover),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, dewpoint, kDewPoint),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, humidity, kHumidity),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, pressure, kPressure),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, rainrate, kRainRate),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, refresh, kRefresh),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, sensordescription,
                                kSensorDescription),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, skybrightness, kSkyBrightness),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, skyquality, kSkyQuality),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, skytemperature, kSkyTemperature),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, starfullwidthhalfmax,
                                kStarFullWidthHalfMax),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, temperature, kTemperature),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, timesincelastupdate,
                                kTimeSinceLastUpdate),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, winddirection, kWindDirection),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, windgust, kWindGust),
        MAKE_ENUM_LITERAL_TOKEN(EDeviceMethod, windspeed, kWindSpeed),
};

bool MatchObservingConditionsMethod(const StringView& view,
                                    EDeviceMethod& match) {
  auto* match_ptr = reinterpret_cast<EDeviceMethod_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(
      view, MakeArrayView(kObservingConditionsMethods), *match_ptr);
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
        // We don't automatically return an error right here because our goal is
        // to return kHttpOk from the decoder if the request is well formed,
        // i.e. we don't want to decide too early that a kHttpNotFound error
        // should be returned.
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

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EParameter_UnderlyingType>
    kRecognizedParameters[] = {
        MAKE_ENUM_LITERAL_TOKEN(EParameter, ClientId, kClientId),
        MAKE_ENUM_LITERAL_TOKEN(EParameter, ClientTransactionId,
                                kClientTransactionId),
        MAKE_ENUM_LITERAL_TOKEN(EParameter, Connected, kConnected),
        MAKE_ENUM_LITERAL_TOKEN(EParameter, SensorName, kSensorName),
};
}  // namespace

bool MatchParameter(const StringView& view, EParameter& match) {
  auto* match_ptr = reinterpret_cast<EParameter_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensCaseInsensitively(
      view, MakeArrayView(kRecognizedParameters), *match_ptr);
}

namespace {
TAS_CONSTEXPR_VAR LiteralToken<ESensorName_UnderlyingType>
    kRecognizedSensorNames[] = {
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, cloudcover, kCloudCover),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, dewpoint, kDewPoint),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, humidity, kHumidity),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, pressure, kPressure),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, rainrate, kRainRate),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, skybrightness, kSkyBrightness),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, skyquality, kSkyQuality),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, skytemperature, kSkyTemperature),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, starfullwidthhalfmax,
                                kStarFullWidthHalfMax),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, temperature, kTemperature),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, winddirection, kWindDirection),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, windgust, kWindGust),
        MAKE_ENUM_LITERAL_TOKEN(ESensorName, windspeed, kWindSpeed),
};
}  // namespace

bool MatchSensorName(const StringView& view, ESensorName& match) {
  auto* match_ptr = reinterpret_cast<ESensorName_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensCaseInsensitively(
      view, MakeArrayView(kRecognizedSensorNames), *match_ptr);
}

namespace {
TAS_CONSTEXPR_VAR LiteralToken<EHttpHeader_UnderlyingType>
    kRecognizedHttpHeaders[] = {
        MAKE_ENUM_LITERAL_TOKEN(EHttpHeader, HttpAccept, kHttpAccept),
        MAKE_ENUM_LITERAL_TOKEN(EHttpHeader, HttpContentLength,
                                kHttpContentLength),
        MAKE_ENUM_LITERAL_TOKEN(EHttpHeader, HttpContentType, kHttpContentType),

        // Content-Encoding is used in tests as an example of a header we know
        // the name of, but don't have built-in handling for the value. It isn't
        // clear whether this is generally useful.
        MAKE_ENUM_LITERAL_TOKEN(EHttpHeader, HttpContentEncoding,
                                kHttpContentEncoding),
};
}  // namespace

bool MatchHttpHeader(const StringView& view, EHttpHeader& match) {
  auto* match_ptr = reinterpret_cast<EHttpHeader_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensCaseInsensitively(
      view, MakeArrayView(kRecognizedHttpHeaders), *match_ptr);
}

#else

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

#endif

}  // namespace alpaca
