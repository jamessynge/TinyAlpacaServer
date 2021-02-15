// GENERATED FILE

#include "alpaca-decoder/constants.h"

#include <type_traits>

namespace alpaca {

std::ostream& operator<<(std::ostream& out, EDecodeStatus v) {
  switch (v) {
    case EDecodeStatus::kContinueDecoding:
      return out << "kContinueDecoding";
    case EDecodeStatus::kNeedMoreInput:
      return out << "kNeedMoreInput";
    case EDecodeStatus::kHttpOk:
      return out << "kHttpOk";
    case EDecodeStatus::kHttpBadRequest:
      return out << "kHttpBadRequest";
    case EDecodeStatus::kHttpNotFound:
      return out << "kHttpNotFound";
    case EDecodeStatus::kHttpMethodNotAllowed:
      return out << "kHttpMethodNotAllowed";
    case EDecodeStatus::kHttpNotAcceptable:
      return out << "kHttpNotAcceptable";
    case EDecodeStatus::kHttpLengthRequired:
      return out << "kHttpLengthRequired";
    case EDecodeStatus::kHttpPayloadTooLarge:
      return out << "kHttpPayloadTooLarge";
    case EDecodeStatus::kHttpUnsupportedMediaType:
      return out << "kHttpUnsupportedMediaType";
    case EDecodeStatus::kHttpRequestHeaderFieldsTooLarge:
      return out << "kHttpRequestHeaderFieldsTooLarge";
    case EDecodeStatus::kInternalError:
      return out << "kInternalError";
    case EDecodeStatus::kHttpMethodNotImplemented:
      return out << "kHttpMethodNotImplemented";
    case EDecodeStatus::kHttpVersionNotSupported:
      return out << "kHttpVersionNotSupported";
  }
  auto invalid = static_cast<std::underlying_type<EDecodeStatus>::type>(v);
  return out << invalid << " (invalid EDecodeStatus)";
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

// std::ostream& operator<<(std::ostream& out, EHttpVersion v) {
//   switch (v) {
//     case EHttpVersion::kUnknown:
//       return out << "kUnknown";
//     case EHttpVersion::kHttp11:
//       return out << "kHttp11";
//   }
//   auto invalid = static_cast<std::underlying_type<EHttpVersion>::type>(v);
//   return out << invalid << " (invalid EHttpVersion)";
// }

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

std::ostream& operator<<(std::ostream& out, EMethod v) {
  switch (v) {
    case EMethod::kUnknown:
      return out << "kUnknown";
    case EMethod::kConnected:
      return out << "kConnected";
    case EMethod::kDescription:
      return out << "kDescription";
    case EMethod::kDriverInfo:
      return out << "kDriverInfo";
    case EMethod::kDriverVersion:
      return out << "kDriverVersion";
    case EMethod::kInterfaceVersion:
      return out << "kInterfaceVersion";
    case EMethod::kNames:
      return out << "kNames";
    case EMethod::kSupportedActions:
      return out << "kSupportedActions";
    case EMethod::kAveragePeriod:
      return out << "kAveragePeriod";
    case EMethod::kCloudCover:
      return out << "kCloudCover";
    case EMethod::kDewPoint:
      return out << "kDewPoint";
    case EMethod::kHumidity:
      return out << "kHumidity";
    case EMethod::kPressure:
      return out << "kPressure";
    case EMethod::kRainRate:
      return out << "kRainRate";
    case EMethod::kRefresh:
      return out << "kRefresh";
    case EMethod::kTemperature:
      return out << "kTemperature";
    case EMethod::kSensorDescription:
      return out << "kSensorDescription";
    case EMethod::kIsSafe:
      return out << "kIsSafe";
  }
  auto invalid = static_cast<std::underlying_type<EMethod>::type>(v);
  return out << invalid << " (invalid EMethod)";
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
  }
  auto invalid = static_cast<std::underlying_type<EHttpHeader>::type>(v);
  return out << invalid << " (invalid EHttpHeader)";
}

}  // namespace alpaca
