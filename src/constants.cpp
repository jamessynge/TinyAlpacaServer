// GENERATED FILE (mostly)

#include "constants.h"

namespace alpaca {

namespace {

size_t PrintUnknownEnumValueTo(const char* name, uint32_t v, Print& out) {
  size_t result = out.print("Unknown ");
  result += out.print(name);
  result += out.print(" (");
  result += out.print(v);
  result += out.print(")");
  return result;
}
}  // namespace

size_t PrintRequestDecoderStatusTo(RequestDecoderStatus v, Print& out) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return out.print("kReset");
    case RequestDecoderStatus::kDecoding:
      return out.print("kDecoding");
    case RequestDecoderStatus::kDecoded:
      return out.print("kDecoded");
  }
  return PrintUnknownEnumValueTo("RequestDecoderStatus",
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpStatusCode v, Print& out) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return out.print("kContinueDecoding");
    case EHttpStatusCode::kNeedMoreInput:
      return out.print("kNeedMoreInput");
    case EHttpStatusCode::kHttpOk:
      return out.print("kHttpOk");
    case EHttpStatusCode::kHttpBadRequest:
      return out.print("kHttpBadRequest");
    case EHttpStatusCode::kHttpNotFound:
      return out.print("kHttpNotFound");
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return out.print("kHttpMethodNotAllowed");
    case EHttpStatusCode::kHttpNotAcceptable:
      return out.print("kHttpNotAcceptable");
    case EHttpStatusCode::kHttpLengthRequired:
      return out.print("kHttpLengthRequired");
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return out.print("kHttpPayloadTooLarge");
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return out.print("kHttpUnsupportedMediaType");
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return out.print("kHttpRequestHeaderFieldsTooLarge");
    case EHttpStatusCode::kHttpInternalServerError:
      return out.print("kHttpInternalServerError");
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return out.print("kHttpMethodNotImplemented");
    case EHttpStatusCode::kHttpVersionNotSupported:
      return out.print("kHttpVersionNotSupported");
  }
  return PrintUnknownEnumValueTo("EHttpStatusCode", static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EHttpMethod v, Print& out) {
  switch (v) {
    case EHttpMethod::kUnknown:
      return out.print("kUnknown");
    case EHttpMethod::GET:
      return out.print("GET");
    case EHttpMethod::PUT:
      return out.print("PUT");
    case EHttpMethod::HEAD:
      return out.print("HEAD");
  }
  return PrintUnknownEnumValueTo("EHttpMethod", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EApiGroup v, Print& out) {
  switch (v) {
    case EApiGroup::kUnknown:
      return out.print("kUnknown");
    case EApiGroup::kDevice:
      return out.print("kDevice");
    case EApiGroup::kManagement:
      return out.print("kManagement");
    case EApiGroup::kSetup:
      return out.print("kSetup");
  }
  return PrintUnknownEnumValueTo("EApiGroup", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EAlpacaApi v, Print& out) {
  switch (v) {
    case EAlpacaApi::kUnknown:
      return out.print("kUnknown");
    case EAlpacaApi::kDeviceApi:
      return out.print("kDeviceApi");
    case EAlpacaApi::kDeviceSetup:
      return out.print("kDeviceSetup");
    case EAlpacaApi::kManagementApiVersions:
      return out.print("kManagementApiVersions");
    case EAlpacaApi::kManagementDescription:
      return out.print("kManagementDescription");
    case EAlpacaApi::kManagementConfiguredDevices:
      return out.print("kManagementConfiguredDevices");
    case EAlpacaApi::kServerSetup:
      return out.print("kServerSetup");
  }
  return PrintUnknownEnumValueTo("EAlpacaApi", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EManagementMethod v, Print& out) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return out.print("kUnknown");
    case EManagementMethod::kDescription:
      return out.print("kDescription");
    case EManagementMethod::kConfiguredDevices:
      return out.print("kConfiguredDevices");
  }
  return PrintUnknownEnumValueTo("EManagementMethod", static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EDeviceType v, Print& out) {
  switch (v) {
    case EDeviceType::kUnknown:
      return out.print("kUnknown");
    case EDeviceType::kCamera:
      return out.print("kCamera");
    case EDeviceType::kCoverCalibrator:
      return out.print("kCoverCalibrator");
    case EDeviceType::kDome:
      return out.print("kDome");
    case EDeviceType::kFilterWheel:
      return out.print("kFilterWheel");
    case EDeviceType::kFocuser:
      return out.print("kFocuser");
    case EDeviceType::kObservingConditions:
      return out.print("kObservingConditions");
    case EDeviceType::kRotator:
      return out.print("kRotator");
    case EDeviceType::kSafetyMonitor:
      return out.print("kSafetyMonitor");
    case EDeviceType::kSwitch:
      return out.print("kSwitch");
    case EDeviceType::kTelescope:
      return out.print("kTelescope");
  }
  return PrintUnknownEnumValueTo("EDeviceType", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceMethod v, Print& out) {
  switch (v) {
    case EDeviceMethod::kUnknown:
      return out.print("kUnknown");
    case EDeviceMethod::kSetup:
      return out.print("kSetup");
    case EDeviceMethod::kConnected:
      return out.print("kConnected");
    case EDeviceMethod::kDescription:
      return out.print("kDescription");
    case EDeviceMethod::kDriverInfo:
      return out.print("kDriverInfo");
    case EDeviceMethod::kDriverVersion:
      return out.print("kDriverVersion");
    case EDeviceMethod::kInterfaceVersion:
      return out.print("kInterfaceVersion");
    case EDeviceMethod::kName:
      return out.print("kName");
    case EDeviceMethod::kSupportedActions:
      return out.print("kSupportedActions");
    case EDeviceMethod::kAveragePeriod:
      return out.print("kAveragePeriod");
    case EDeviceMethod::kCloudCover:
      return out.print("kCloudCover");
    case EDeviceMethod::kDewPoint:
      return out.print("kDewPoint");
    case EDeviceMethod::kHumidity:
      return out.print("kHumidity");
    case EDeviceMethod::kPressure:
      return out.print("kPressure");
    case EDeviceMethod::kRainRate:
      return out.print("kRainRate");
    case EDeviceMethod::kRefresh:
      return out.print("kRefresh");
    case EDeviceMethod::kSensorDescription:
      return out.print("kSensorDescription");
    case EDeviceMethod::kSkyBrightness:
      return out.print("kSkyBrightness");
    case EDeviceMethod::kSkyQuality:
      return out.print("kSkyQuality");
    case EDeviceMethod::kSkyTemperature:
      return out.print("kSkyTemperature");
    case EDeviceMethod::kStarFullWidthHalfMax:
      return out.print("kStarFullWidthHalfMax");
    case EDeviceMethod::kTemperature:
      return out.print("kTemperature");
    case EDeviceMethod::kTimeSinceLastUpdate:
      return out.print("kTimeSinceLastUpdate");
    case EDeviceMethod::kWindDirection:
      return out.print("kWindDirection");
    case EDeviceMethod::kWindGust:
      return out.print("kWindGust");
    case EDeviceMethod::kWindSpeed:
      return out.print("kWindSpeed");
    case EDeviceMethod::kIsSafe:
      return out.print("kIsSafe");
  }
  return PrintUnknownEnumValueTo("EDeviceMethod", static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EParameter v, Print& out) {
  switch (v) {
    case EParameter::kUnknown:
      return out.print("kUnknown");
    case EParameter::kClientId:
      return out.print("kClientId");
    case EParameter::kClientTransactionId:
      return out.print("kClientTransactionId");
    case EParameter::kConnected:
      return out.print("kConnected");
    case EParameter::kSensorName:
      return out.print("kSensorName");
  }
  return PrintUnknownEnumValueTo("EParameter", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(ESensorName v, Print& out) {
  switch (v) {
    case ESensorName::kUnknown:
      return out.print("kUnknown");
    case ESensorName::kCloudCover:
      return out.print("kCloudCover");
    case ESensorName::kDewPoint:
      return out.print("kDewPoint");
    case ESensorName::kHumidity:
      return out.print("kHumidity");
    case ESensorName::kPressure:
      return out.print("kPressure");
    case ESensorName::kRainRate:
      return out.print("kRainRate");
    case ESensorName::kSkyBrightness:
      return out.print("kSkyBrightness");
    case ESensorName::kSkyQuality:
      return out.print("kSkyQuality");
    case ESensorName::kSkyTemperature:
      return out.print("kSkyTemperature");
    case ESensorName::kStarFullWidthHalfMax:
      return out.print("kStarFullWidthHalfMax");
    case ESensorName::kTemperature:
      return out.print("kTemperature");
    case ESensorName::kWindDirection:
      return out.print("kWindDirection");
    case ESensorName::kWindGust:
      return out.print("kWindGust");
    case ESensorName::kWindSpeed:
      return out.print("kWindSpeed");
  }
  return PrintUnknownEnumValueTo("ESensorName", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpHeader v, Print& out) {
  switch (v) {
    case EHttpHeader::kUnknown:
      return out.print("kUnknown");
    case EHttpHeader::kHttpAccept:
      return out.print("kHttpAccept");
    case EHttpHeader::kHttpContentLength:
      return out.print("kHttpContentLength");
    case EHttpHeader::kHttpContentType:
      return out.print("kHttpContentType");
    case EHttpHeader::kHttpContentEncoding:
      return out.print("kHttpContentEncoding");
  }
  return PrintUnknownEnumValueTo("EHttpHeader", static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EContentType v, Print& out) {
  switch (v) {
    case EContentType::kApplicationJson:
      return out.print("kApplicationJson");
    case EContentType::kTextPlain:
      return out.print("kTextPlain");
  }
  return PrintUnknownEnumValueTo("EContentType", static_cast<uint32_t>(v), out);
}

}  // namespace alpaca

#if TAS_HOST_TARGET
#include <type_traits>

namespace alpaca {

std::ostream& operator<<(std::ostream& out, RequestDecoderStatus v) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return out << "kReset";
    case RequestDecoderStatus::kDecoding:
      return out << "kDecoding";
    case RequestDecoderStatus::kDecoded:
      return out << "kDecoded";
  }
  auto invalid =
      static_cast<std::underlying_type<RequestDecoderStatus>::type>(v);
  return out << invalid << " (invalid RequestDecoderStatus)";
}

std::ostream& operator<<(std::ostream& out, EHttpStatusCode v) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return out << "kContinueDecoding";
    case EHttpStatusCode::kNeedMoreInput:
      return out << "kNeedMoreInput";
    case EHttpStatusCode::kHttpOk:
      return out << "kHttpOk";
    case EHttpStatusCode::kHttpBadRequest:
      return out << "kHttpBadRequest";
    case EHttpStatusCode::kHttpNotFound:
      return out << "kHttpNotFound";
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return out << "kHttpMethodNotAllowed";
    case EHttpStatusCode::kHttpNotAcceptable:
      return out << "kHttpNotAcceptable";
    case EHttpStatusCode::kHttpLengthRequired:
      return out << "kHttpLengthRequired";
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return out << "kHttpPayloadTooLarge";
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return out << "kHttpUnsupportedMediaType";
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return out << "kHttpRequestHeaderFieldsTooLarge";
    case EHttpStatusCode::kHttpInternalServerError:
      return out << "kHttpInternalServerError";
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return out << "kHttpMethodNotImplemented";
    case EHttpStatusCode::kHttpVersionNotSupported:
      return out << "kHttpVersionNotSupported";
  }
  auto invalid = static_cast<std::underlying_type<EHttpStatusCode>::type>(v);
  return out << invalid << " (invalid EHttpStatusCode)";
}

std::ostream& operator<<(std::ostream& out, EHttpMethod v) {
  switch (v) {
    case EHttpMethod::kUnknown:
      return out << "kUnknown";
    case EHttpMethod::GET:
      return out << "GET";
    case EHttpMethod::PUT:
      return out << "PUT";
    case EHttpMethod::HEAD:
      return out << "HEAD";
  }
  auto invalid = static_cast<std::underlying_type<EHttpMethod>::type>(v);
  return out << invalid << " (invalid EHttpMethod)";
}

std::ostream& operator<<(std::ostream& out, EApiGroup v) {
  switch (v) {
    case EApiGroup::kUnknown:
      return out << "kUnknown";
    case EApiGroup::kDevice:
      return out << "kDevice";
    case EApiGroup::kManagement:
      return out << "kManagement";
    case EApiGroup::kSetup:
      return out << "kSetup";
  }
  auto invalid = static_cast<std::underlying_type<EApiGroup>::type>(v);
  return out << invalid << " (invalid EApiGroup)";
}

std::ostream& operator<<(std::ostream& out, EAlpacaApi v) {
  switch (v) {
    case EAlpacaApi::kUnknown:
      return out << "kUnknown";
    case EAlpacaApi::kDeviceApi:
      return out << "kDeviceApi";
    case EAlpacaApi::kDeviceSetup:
      return out << "kDeviceSetup";
    case EAlpacaApi::kManagementApiVersions:
      return out << "kManagementApiVersions";
    case EAlpacaApi::kManagementDescription:
      return out << "kManagementDescription";
    case EAlpacaApi::kManagementConfiguredDevices:
      return out << "kManagementConfiguredDevices";
    case EAlpacaApi::kServerSetup:
      return out << "kServerSetup";
  }
  auto invalid = static_cast<std::underlying_type<EAlpacaApi>::type>(v);
  return out << invalid << " (invalid EAlpacaApi)";
}

std::ostream& operator<<(std::ostream& out, EManagementMethod v) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return out << "kUnknown";
    case EManagementMethod::kDescription:
      return out << "kDescription";
    case EManagementMethod::kConfiguredDevices:
      return out << "kConfiguredDevices";
  }
  auto invalid = static_cast<std::underlying_type<EManagementMethod>::type>(v);
  return out << invalid << " (invalid EManagementMethod)";
}

std::ostream& operator<<(std::ostream& out, EDeviceType v) {
  switch (v) {
    case EDeviceType::kUnknown:
      return out << "kUnknown";
    case EDeviceType::kCamera:
      return out << "kCamera";
    case EDeviceType::kCoverCalibrator:
      return out << "kCoverCalibrator";
    case EDeviceType::kDome:
      return out << "kDome";
    case EDeviceType::kFilterWheel:
      return out << "kFilterWheel";
    case EDeviceType::kFocuser:
      return out << "kFocuser";
    case EDeviceType::kObservingConditions:
      return out << "kObservingConditions";
    case EDeviceType::kRotator:
      return out << "kRotator";
    case EDeviceType::kSafetyMonitor:
      return out << "kSafetyMonitor";
    case EDeviceType::kSwitch:
      return out << "kSwitch";
    case EDeviceType::kTelescope:
      return out << "kTelescope";
  }
  auto invalid = static_cast<std::underlying_type<EDeviceType>::type>(v);
  return out << invalid << " (invalid EDeviceType)";
}

std::ostream& operator<<(std::ostream& out, EDeviceMethod v) {
  switch (v) {
    case EDeviceMethod::kUnknown:
      return out << "kUnknown";
    case EDeviceMethod::kSetup:
      return out << "kSetup";
    case EDeviceMethod::kConnected:
      return out << "kConnected";
    case EDeviceMethod::kDescription:
      return out << "kDescription";
    case EDeviceMethod::kDriverInfo:
      return out << "kDriverInfo";
    case EDeviceMethod::kDriverVersion:
      return out << "kDriverVersion";
    case EDeviceMethod::kInterfaceVersion:
      return out << "kInterfaceVersion";
    case EDeviceMethod::kName:
      return out << "kName";
    case EDeviceMethod::kSupportedActions:
      return out << "kSupportedActions";
    case EDeviceMethod::kAveragePeriod:
      return out << "kAveragePeriod";
    case EDeviceMethod::kCloudCover:
      return out << "kCloudCover";
    case EDeviceMethod::kDewPoint:
      return out << "kDewPoint";
    case EDeviceMethod::kHumidity:
      return out << "kHumidity";
    case EDeviceMethod::kPressure:
      return out << "kPressure";
    case EDeviceMethod::kRainRate:
      return out << "kRainRate";
    case EDeviceMethod::kRefresh:
      return out << "kRefresh";
    case EDeviceMethod::kSensorDescription:
      return out << "kSensorDescription";
    case EDeviceMethod::kSkyBrightness:
      return out << "kSkyBrightness";
    case EDeviceMethod::kSkyQuality:
      return out << "kSkyQuality";
    case EDeviceMethod::kSkyTemperature:
      return out << "kSkyTemperature";
    case EDeviceMethod::kStarFullWidthHalfMax:
      return out << "kStarFullWidthHalfMax";
    case EDeviceMethod::kTemperature:
      return out << "kTemperature";
    case EDeviceMethod::kTimeSinceLastUpdate:
      return out << "kTimeSinceLastUpdate";
    case EDeviceMethod::kWindDirection:
      return out << "kWindDirection";
    case EDeviceMethod::kWindGust:
      return out << "kWindGust";
    case EDeviceMethod::kWindSpeed:
      return out << "kWindSpeed";
    case EDeviceMethod::kIsSafe:
      return out << "kIsSafe";
  }
  auto invalid = static_cast<std::underlying_type<EDeviceMethod>::type>(v);
  return out << invalid << " (invalid EDeviceMethod)";
}

std::ostream& operator<<(std::ostream& out, EParameter v) {
  switch (v) {
    case EParameter::kUnknown:
      return out << "kUnknown";
    case EParameter::kClientId:
      return out << "kClientId";
    case EParameter::kClientTransactionId:
      return out << "kClientTransactionId";
    case EParameter::kConnected:
      return out << "kConnected";
    case EParameter::kSensorName:
      return out << "kSensorName";
  }
  auto invalid = static_cast<std::underlying_type<EParameter>::type>(v);
  return out << invalid << " (invalid EParameter)";
}

std::ostream& operator<<(std::ostream& out, ESensorName v) {
  switch (v) {
    case ESensorName::kUnknown:
      return out << "kUnknown";
    case ESensorName::kCloudCover:
      return out << "kCloudCover";
    case ESensorName::kDewPoint:
      return out << "kDewPoint";
    case ESensorName::kHumidity:
      return out << "kHumidity";
    case ESensorName::kPressure:
      return out << "kPressure";
    case ESensorName::kRainRate:
      return out << "kRainRate";
    case ESensorName::kSkyBrightness:
      return out << "kSkyBrightness";
    case ESensorName::kSkyQuality:
      return out << "kSkyQuality";
    case ESensorName::kSkyTemperature:
      return out << "kSkyTemperature";
    case ESensorName::kStarFullWidthHalfMax:
      return out << "kStarFullWidthHalfMax";
    case ESensorName::kTemperature:
      return out << "kTemperature";
    case ESensorName::kWindDirection:
      return out << "kWindDirection";
    case ESensorName::kWindGust:
      return out << "kWindGust";
    case ESensorName::kWindSpeed:
      return out << "kWindSpeed";
  }
  auto invalid = static_cast<std::underlying_type<ESensorName>::type>(v);
  return out << invalid << " (invalid ESensorName)";
}

std::ostream& operator<<(std::ostream& out, EHttpHeader v) {
  switch (v) {
    case EHttpHeader::kUnknown:
      return out << "kUnknown";
    case EHttpHeader::kHttpAccept:
      return out << "kHttpAccept";
    case EHttpHeader::kHttpContentLength:
      return out << "kHttpContentLength";
    case EHttpHeader::kHttpContentType:
      return out << "kHttpContentType";
    case EHttpHeader::kHttpContentEncoding:
      return out << "kHttpContentEncoding";
  }
  auto invalid = static_cast<std::underlying_type<EHttpHeader>::type>(v);
  return out << invalid << " (invalid EHttpHeader)";
}

}  // namespace alpaca
#endif  // TAS_HOST_TARGET
