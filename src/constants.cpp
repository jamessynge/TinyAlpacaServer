#include "constants.h"

// GENERATED FILE (mostly)

#include "utils/inline_literal.h"

namespace alpaca {

namespace {

size_t PrintUnknownEnumValueTo(const Printable& name, uint32_t v, Print& out) {
  size_t result = out.print(TASLIT("Unknown "));
  result += out.print(name);
  result += out.print(TASLIT(" ("));
  result += out.print(v);
  result += out.print(TASLIT(")"));
  return result;
}
}  // namespace

size_t PrintRequestDecoderStatusTo(RequestDecoderStatus v, Print& out) {
  switch (v) {
    case RequestDecoderStatus::kReset:
      return out.print(TASLIT("kReset"));
    case RequestDecoderStatus::kDecoding:
      return out.print(TASLIT("kDecoding"));
    case RequestDecoderStatus::kDecoded:
      return out.print(TASLIT("kDecoded"));
  }
  return PrintUnknownEnumValueTo(TASLIT("RequestDecoderStatus"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpStatusCode v, Print& out) {
  switch (v) {
    case EHttpStatusCode::kContinueDecoding:
      return out.print(TASLIT("kContinueDecoding"));
    case EHttpStatusCode::kNeedMoreInput:
      return out.print(TASLIT("kNeedMoreInput"));
    case EHttpStatusCode::kHttpOk:
      return out.print(TASLIT("kHttpOk"));
    case EHttpStatusCode::kHttpBadRequest:
      return out.print(TASLIT("kHttpBadRequest"));
    case EHttpStatusCode::kHttpNotFound:
      return out.print(TASLIT("kHttpNotFound"));
    case EHttpStatusCode::kHttpMethodNotAllowed:
      return out.print(TASLIT("kHttpMethodNotAllowed"));
    case EHttpStatusCode::kHttpNotAcceptable:
      return out.print(TASLIT("kHttpNotAcceptable"));
    case EHttpStatusCode::kHttpLengthRequired:
      return out.print(TASLIT("kHttpLengthRequired"));
    case EHttpStatusCode::kHttpPayloadTooLarge:
      return out.print(TASLIT("kHttpPayloadTooLarge"));
    case EHttpStatusCode::kHttpUnsupportedMediaType:
      return out.print(TASLIT("kHttpUnsupportedMediaType"));
    case EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge:
      return out.print(TASLIT("kHttpRequestHeaderFieldsTooLarge"));
    case EHttpStatusCode::kHttpInternalServerError:
      return out.print(TASLIT("kHttpInternalServerError"));
    case EHttpStatusCode::kHttpMethodNotImplemented:
      return out.print(TASLIT("kHttpMethodNotImplemented"));
    case EHttpStatusCode::kHttpVersionNotSupported:
      return out.print(TASLIT("kHttpVersionNotSupported"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EHttpStatusCode"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpMethod v, Print& out) {
  switch (v) {
    case EHttpMethod::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EHttpMethod::GET:
      return out.print(TASLIT("GET"));
    case EHttpMethod::PUT:
      return out.print(TASLIT("PUT"));
    case EHttpMethod::HEAD:
      return out.print(TASLIT("HEAD"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EHttpMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EApiGroup v, Print& out) {
  switch (v) {
    case EApiGroup::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EApiGroup::kDevice:
      return out.print(TASLIT("kDevice"));
    case EApiGroup::kManagement:
      return out.print(TASLIT("kManagement"));
    case EApiGroup::kSetup:
      return out.print(TASLIT("kSetup"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EApiGroup"), static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EAlpacaApi v, Print& out) {
  switch (v) {
    case EAlpacaApi::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EAlpacaApi::kDeviceApi:
      return out.print(TASLIT("kDeviceApi"));
    case EAlpacaApi::kDeviceSetup:
      return out.print(TASLIT("kDeviceSetup"));
    case EAlpacaApi::kManagementApiVersions:
      return out.print(TASLIT("kManagementApiVersions"));
    case EAlpacaApi::kManagementDescription:
      return out.print(TASLIT("kManagementDescription"));
    case EAlpacaApi::kManagementConfiguredDevices:
      return out.print(TASLIT("kManagementConfiguredDevices"));
    case EAlpacaApi::kServerSetup:
      return out.print(TASLIT("kServerSetup"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EAlpacaApi"), static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(EManagementMethod v, Print& out) {
  switch (v) {
    case EManagementMethod::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EManagementMethod::kDescription:
      return out.print(TASLIT("kDescription"));
    case EManagementMethod::kConfiguredDevices:
      return out.print(TASLIT("kConfiguredDevices"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EManagementMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceType v, Print& out) {
  switch (v) {
    case EDeviceType::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EDeviceType::kCamera:
      return out.print(TASLIT("kCamera"));
    case EDeviceType::kCoverCalibrator:
      return out.print(TASLIT("kCoverCalibrator"));
    case EDeviceType::kDome:
      return out.print(TASLIT("kDome"));
    case EDeviceType::kFilterWheel:
      return out.print(TASLIT("kFilterWheel"));
    case EDeviceType::kFocuser:
      return out.print(TASLIT("kFocuser"));
    case EDeviceType::kObservingConditions:
      return out.print(TASLIT("kObservingConditions"));
    case EDeviceType::kRotator:
      return out.print(TASLIT("kRotator"));
    case EDeviceType::kSafetyMonitor:
      return out.print(TASLIT("kSafetyMonitor"));
    case EDeviceType::kSwitch:
      return out.print(TASLIT("kSwitch"));
    case EDeviceType::kTelescope:
      return out.print(TASLIT("kTelescope"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EDeviceType"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EDeviceMethod v, Print& out) {
  switch (v) {
    case EDeviceMethod::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EDeviceMethod::kSetup:
      return out.print(TASLIT("kSetup"));
    case EDeviceMethod::kConnected:
      return out.print(TASLIT("kConnected"));
    case EDeviceMethod::kDescription:
      return out.print(TASLIT("kDescription"));
    case EDeviceMethod::kDriverInfo:
      return out.print(TASLIT("kDriverInfo"));
    case EDeviceMethod::kDriverVersion:
      return out.print(TASLIT("kDriverVersion"));
    case EDeviceMethod::kInterfaceVersion:
      return out.print(TASLIT("kInterfaceVersion"));
    case EDeviceMethod::kName:
      return out.print(TASLIT("kName"));
    case EDeviceMethod::kSupportedActions:
      return out.print(TASLIT("kSupportedActions"));
    case EDeviceMethod::kAveragePeriod:
      return out.print(TASLIT("kAveragePeriod"));
    case EDeviceMethod::kCloudCover:
      return out.print(TASLIT("kCloudCover"));
    case EDeviceMethod::kDewPoint:
      return out.print(TASLIT("kDewPoint"));
    case EDeviceMethod::kHumidity:
      return out.print(TASLIT("kHumidity"));
    case EDeviceMethod::kPressure:
      return out.print(TASLIT("kPressure"));
    case EDeviceMethod::kRainRate:
      return out.print(TASLIT("kRainRate"));
    case EDeviceMethod::kRefresh:
      return out.print(TASLIT("kRefresh"));
    case EDeviceMethod::kSensorDescription:
      return out.print(TASLIT("kSensorDescription"));
    case EDeviceMethod::kSkyBrightness:
      return out.print(TASLIT("kSkyBrightness"));
    case EDeviceMethod::kSkyQuality:
      return out.print(TASLIT("kSkyQuality"));
    case EDeviceMethod::kSkyTemperature:
      return out.print(TASLIT("kSkyTemperature"));
    case EDeviceMethod::kStarFullWidthHalfMax:
      return out.print(TASLIT("kStarFullWidthHalfMax"));
    case EDeviceMethod::kTemperature:
      return out.print(TASLIT("kTemperature"));
    case EDeviceMethod::kTimeSinceLastUpdate:
      return out.print(TASLIT("kTimeSinceLastUpdate"));
    case EDeviceMethod::kWindDirection:
      return out.print(TASLIT("kWindDirection"));
    case EDeviceMethod::kWindGust:
      return out.print(TASLIT("kWindGust"));
    case EDeviceMethod::kWindSpeed:
      return out.print(TASLIT("kWindSpeed"));
    case EDeviceMethod::kIsSafe:
      return out.print(TASLIT("kIsSafe"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EDeviceMethod"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EParameter v, Print& out) {
  switch (v) {
    case EParameter::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EParameter::kClientId:
      return out.print(TASLIT("kClientId"));
    case EParameter::kClientTransactionId:
      return out.print(TASLIT("kClientTransactionId"));
    case EParameter::kConnected:
      return out.print(TASLIT("kConnected"));
    case EParameter::kSensorName:
      return out.print(TASLIT("kSensorName"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EParameter"), static_cast<uint32_t>(v),
                                 out);
}

size_t PrintValueTo(ESensorName v, Print& out) {
  switch (v) {
    case ESensorName::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case ESensorName::kCloudCover:
      return out.print(TASLIT("kCloudCover"));
    case ESensorName::kDewPoint:
      return out.print(TASLIT("kDewPoint"));
    case ESensorName::kHumidity:
      return out.print(TASLIT("kHumidity"));
    case ESensorName::kPressure:
      return out.print(TASLIT("kPressure"));
    case ESensorName::kRainRate:
      return out.print(TASLIT("kRainRate"));
    case ESensorName::kSkyBrightness:
      return out.print(TASLIT("kSkyBrightness"));
    case ESensorName::kSkyQuality:
      return out.print(TASLIT("kSkyQuality"));
    case ESensorName::kSkyTemperature:
      return out.print(TASLIT("kSkyTemperature"));
    case ESensorName::kStarFullWidthHalfMax:
      return out.print(TASLIT("kStarFullWidthHalfMax"));
    case ESensorName::kTemperature:
      return out.print(TASLIT("kTemperature"));
    case ESensorName::kWindDirection:
      return out.print(TASLIT("kWindDirection"));
    case ESensorName::kWindGust:
      return out.print(TASLIT("kWindGust"));
    case ESensorName::kWindSpeed:
      return out.print(TASLIT("kWindSpeed"));
  }
  return PrintUnknownEnumValueTo(TASLIT("ESensorName"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EHttpHeader v, Print& out) {
  switch (v) {
    case EHttpHeader::kUnknown:
      return out.print(TASLIT("kUnknown"));
    case EHttpHeader::kHttpAccept:
      return out.print(TASLIT("kHttpAccept"));
    case EHttpHeader::kHttpContentLength:
      return out.print(TASLIT("kHttpContentLength"));
    case EHttpHeader::kHttpContentType:
      return out.print(TASLIT("kHttpContentType"));
    case EHttpHeader::kHttpContentEncoding:
      return out.print(TASLIT("kHttpContentEncoding"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EHttpHeader"),
                                 static_cast<uint32_t>(v), out);
}

size_t PrintValueTo(EContentType v, Print& out) {
  switch (v) {
    case EContentType::kApplicationJson:
      return out.print(TASLIT("kApplicationJson"));
    case EContentType::kTextPlain:
      return out.print(TASLIT("kTextPlain"));
  }
  return PrintUnknownEnumValueTo(TASLIT("EContentType"),
                                 static_cast<uint32_t>(v), out);
}

}  // namespace alpaca
