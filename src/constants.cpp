// GENERATED FILE

#include "constants.h"

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
  }
  auto invalid = static_cast<std::underlying_type<EParameter>::type>(v);
  return out << invalid << " (invalid EParameter)";
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
