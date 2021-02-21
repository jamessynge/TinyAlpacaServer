#ifndef TINY_ALPACA_SERVER_DECODER_CONSTANTS_H_
#define TINY_ALPACA_SERVER_DECODER_CONSTANTS_H_

// Enums defining the decoder return values and enums corresponding to each kind
// of token to be decoded.
//
// Note that we include the streaming operators in support of testing; these
// should not be used in the embedded code (except in DLOG or DCHECK type
// macro invocations).
//
// Author: james.synge@gmail.com

#include "platform.h"

#if TAS_HOST_TARGET
#include <ostream>
#endif  // TAS_HOST_TARGET

namespace alpaca {

// Some of these are based on HTTP Response Status Codes, the others are below
// 100 (HTTP's continue status).
enum class EDecodeStatus : uint16_t {
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
std::ostream& operator<<(std::ostream& out, EDecodeStatus value);
#endif  // TAS_HOST_TARGET

enum class EHttpMethod : uint8_t {
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

// // DO WE NEED TO KNOW THIS?
// enum class EHttpVersion : uint8_t {
//   kUnknown,
//   kHttp11,  // HTTP/1.1
// };
// std::ostream& operator<<(std::ostream& out, EHttpVersion value);

enum class EDeviceType {
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
// enum in both EMethod and EParameter, and a corresponding Token for each in
// requests.cc.
enum class EMethod : uint8_t {
  kUnknown,

  // Supported common methods:
  kConnected,
  kDescription,
  kDriverInfo,
  kDriverVersion,
  kInterfaceVersion,
  kName,
  kSupportedActions,

  // Supported ObservingConditions methods:
  kAveragePeriod,
  kCloudCover,
  kDewPoint,
  kHumidity,
  kPressure,
  kRainRate,
  kRefresh,
  kTemperature,
  kSensorDescription,

  // Supported SafetyMonitor methods:
  kIsSafe,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EMethod value);
#endif  // TAS_HOST_TARGET

// These are parameter names used in *requests*, not responses. Names such as
// ServerTransactionId and ErrorNumber should not be in this list.
enum class EParameter : uint8_t {
  kUnknown,

  kClientId,
  kClientTransactionId,
  kConnected,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EParameter value);
#endif  // TAS_HOST_TARGET

enum class EHttpHeader : uint8_t {
  kUnknown,

  // TODO(jamessynge): Consider adding header names for transfer-encoding, etc.
  // so that we can reject PUT requests which contain a body that is encoded
  // in a form we can't decode.
  kHttpAccept,
  kHttpContentLength,
  kHttpContentType,
  // Added to enable testing.
  kHttpContentEncoding,
};
#if TAS_HOST_TARGET
std::ostream& operator<<(std::ostream& out, EHttpHeader value);
#endif  // TAS_HOST_TARGET

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_DECODER_CONSTANTS_H_
