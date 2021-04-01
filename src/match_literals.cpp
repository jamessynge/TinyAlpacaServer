#include "match_literals.h"

// NOTE: This uses reinterpret_cast, which is not safe. Why? I'm aiming to avoid
// excessive code size due to template expansion, so I'd normally use
// std::underlying_type<EnumTypeName>, but on the Arduino we don't have
// <type_traits>.

#include "constants.h"
#include "literals.h"
#include "utils/array_view.h"
#include "utils/literal.h"
#include "utils/literal_token.h"
#include "utils/logging.h"
#include "utils/platform.h"

#define MAKE_ENUM_LITERAL_TOKEN(enum_type, literal_name, enum_value)   \
  LiteralToken<enum_type##_UnderlyingType> {                           \
    Literals::literal_name(),                                          \
        static_cast<enum_type##_UnderlyingType>(enum_type::enum_value) \
  }

#define MATCH_ONE_LITERAL(literal_name, enum_value) \
  if (Literals::literal_name() == view) {           \
    match = enum_value;                             \
    return true;                                    \
  }

namespace alpaca {

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
  MATCH_ONE_LITERAL(issafe, EDeviceMethod::kIsSafe);
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
        TAS_VLOG(2) << "device type (" << device_type << ") is not supported";
        break;
    }
    return internal::MatchCommonDeviceMethod(view, match);
  } else if (group == EApiGroup::kSetup) {
    // NOTE: Not checking whether the device type is supported.
    MATCH_ONE_LITERAL(setup, EDeviceMethod::kSetup);
    return false;
  } else {
    TAS_CHECK(false) << "api group (" << group << ") is not device or setup";
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

}  // namespace alpaca
