#ifndef TINY_ALPACA_SERVER_SRC_CONSTANTS_H_
#define TINY_ALPACA_SERVER_SRC_CONSTANTS_H_

// Enums defining the HTTP status and enums corresponding to each kind of token
// to be decoded.
//
// Note that we include the streaming operators in support of testing; these
// should not be used in the embedded code (except in DLOG or DCHECK type
// macro invocations).
//
// Author: james.synge@gmail.com

#include "utils/platform.h"

#if TAS_HOST_TARGET
#include <ostream>
#endif  // TAS_HOST_TARGET

namespace alpaca {

// At a high-level, what is going on with the decoder?
enum class RequestDecoderStatus : uint8_t {
  kReset,
  kDecoding,
  kDecoded,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, RequestDecoderStatus value);
#endif  // TAS_HOST_TARGET

// Most of these are based on HTTP Response Status Codes, the others are below
// 100 (HTTP's continue status).
enum class EHttpStatusCode : uint16_t {
  // kContinueDecoding is first/zero because it will be the default value
  // returned by gmock, thus saving us the trouble of adding
  // .WillOnce(Return(...)) all over the place.
  kContinueDecoding = 0,

  // The current decode function needs some more input to finish its job. If the
  // input buffer is already full (e.g. we have a 40 byte buffer, and the name
  // or value of a header field or parameter is more than 39 bytes), then the
  // decode function will continually return kNeedMoreInput. If the caller of
  // RequestDecoder::DecodeBuffer has passed true for buffer_is_full, then
  // DecodeBuffer will convert kNeedMoreInput to an error status (e.g.
  // kHttpRequestHeaderFieldsTooLarge).
  kNeedMoreInput,

  // The request has been successfully decoded.
  kHttpOk = 200,

  // Invalid syntax in the request.
  kHttpBadRequest = 400,

  // Can't find the requested resource (e.g. unknown device type or device
  // number).
  kHttpNotFound = 404,

  // Unknown/unsupported HTTP method.
  kHttpMethodNotAllowed = 405,
  // If Accept header from client doesn't include application/json.
  kHttpNotAcceptable = 406,
  kHttpLengthRequired = 411,
  kHttpPayloadTooLarge = 413,
  kHttpUnsupportedMediaType = 415,

  // May use for extra-long names and values.
  kHttpRequestHeaderFieldsTooLarge = 431,

  // Unspecified problem with processing the request.
  kHttpInternalServerError = 500,
  kHttpMethodNotImplemented = 501,
  kHttpVersionNotSupported = 505,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EHttpStatusCode value);
#endif  // TAS_HOST_TARGET

using EHttpMethod_UnderlyingType = uint8_t;
enum class EHttpMethod : EHttpMethod_UnderlyingType {
  kUnknown = 0,
  // The supported HTTP methods. Note that the the HTTP/1.1 standard requires
  // that the methods GET and HEAD are supported.
  GET = 1,
  PUT = 2,
  HEAD = 3,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EHttpMethod value);
#endif  // TAS_HOST_TARGET

using EApiGroup_UnderlyingType = uint8_t;
enum class EApiGroup : uint8_t {
  kUnknown = 0,
  kDevice,      // Path: /api...
  kManagement,  // Path: /management...
  kSetup,       // Path: /setup...
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EApiGroup value);
#endif  // TAS_HOST_TARGET

using EAlpacaApi_UnderlyingType = uint8_t;
enum class EAlpacaApi : uint8_t {
  kUnknown = 0,
  kDeviceApi,    // Path: /api/v1/{device_type}/{device_number}/{method}
  kDeviceSetup,  // Path: /setup/v1/{device_type}/{device_number}/setup
  kManagementApiVersions,        // Path: /management/apiversions
  kManagementDescription,        // Path: /management/v1/description
  kManagementConfiguredDevices,  // Path: /management/v1/configureddevices
  kServerSetup,                  // Path: /setup
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EAlpacaApi value);
#endif  // TAS_HOST_TARGET

using EManagementMethod_UnderlyingType = uint8_t;
enum class EManagementMethod : EManagementMethod_UnderlyingType {
  kUnknown,
  kDescription,
  kConfiguredDevices,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EManagementMethod value);
#endif  // TAS_HOST_TARGET

using EDeviceType_UnderlyingType = uint8_t;
enum class EDeviceType : EDeviceType_UnderlyingType {
  kUnknown,
  kCamera,
  kCoverCalibrator,
  kDome,
  kFilterWheel,
  kFocuser,
  kObservingConditions,
  kRotator,
  kSafetyMonitor,
  kSwitch,
  kTelescope,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EDeviceType value);
#endif  // TAS_HOST_TARGET

// Note that for many PUT methods, the name of the method, which appears in the
// request path, appears also in the parameters (e.g. "AveragePeriod" or
// "Connected"). Note that in the path the name must be lower case, while it may
// be mixed case in the list of parameters. IF we can rely on the compiler and
// linker to share literal strings, then it shouldn't be a problem to define an
// enum in both EDeviceMethod and EParameter, and a corresponding Token for each
// in requests.cc.
using EDeviceMethod_UnderlyingType = uint8_t;
enum class EDeviceMethod : EDeviceMethod_UnderlyingType {
  kUnknown,

  // This is the only method for EAlpacaApi::kDeviceSetup:
  kSetup,

  // Supported common methods:
  kConnected,
  kDescription,
  kDriverInfo,
  kDriverVersion,
  kInterfaceVersion,
  kName,
  kSupportedActions,

  // ObservingConditions methods:
  kAveragePeriod,
  kCloudCover,
  kDewPoint,
  kHumidity,
  kPressure,
  kRainRate,
  kRefresh,
  kSensorDescription,
  kSkyBrightness,
  kSkyQuality,
  kSkyTemperature,
  kStarFullWidthHalfMax,
  kTemperature,
  kTimeSinceLastUpdate,
  kWindDirection,
  kWindGust,
  kWindSpeed,

  // Supported SafetyMonitor methods:
  kIsSafe,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EDeviceMethod value);
#endif  // TAS_HOST_TARGET

// These are parameter names used in *requests*, not responses. Names such as
// ServerTransactionId and ErrorNumber should not be in this list.
using EParameter_UnderlyingType = uint8_t;
enum class EParameter : EParameter_UnderlyingType {
  kUnknown,

  kClientId,
  kClientTransactionId,
  kConnected,
  kSensorName,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EParameter value);
#endif  // TAS_HOST_TARGET

using EHttpHeader_UnderlyingType = uint8_t;
enum class EHttpHeader : EHttpHeader_UnderlyingType {
  kUnknown,

  // TODO(jamessynge): Consider adding header names for transfer-encoding, etc.
  // so that we can reject PUT requests which contain a body that is encoded
  // in a form we can't decode.
  kHttpAccept,
  kHttpContentLength,
  kHttpContentType,

  // Added to enable testing of RequestDecoderListener::OnExtraHeader.
  kHttpContentEncoding,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EHttpHeader value);
#endif  // TAS_HOST_TARGET

// This is used for generating responses, not for input.
enum class EContentType : uint8_t {
  kApplicationJson = 0,
  kTextPlain,
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_CONSTANTS_H_
