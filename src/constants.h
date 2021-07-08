#ifndef TINY_ALPACA_SERVER_SRC_CONSTANTS_H_
#define TINY_ALPACA_SERVER_SRC_CONSTANTS_H_

// Enums defining the HTTP status and enums corresponding to each kind of token
// to be decoded.
//
// Note that we include the streaming operators in support of testing; these
// should not be used in the embedded code (except in DLOG or DCHECK type
// macro invocations).
//
// The <EnumName>_UnderlyingType declarations are present to help with reducing
// the number of unique template instantiations of functions operating on arrays
// of enums, with the aim of reducing the amount of PROGMEM (flash) required.
// The reason for explicitly declaring the underlying type is that the Arduino
// compiler doesn't include the <type_traits> library, and hence the definition
// of std::underlying_type<E>. (Note that the function motivating the addition
// of those underlying type declarations has since been removed, which calls
// into question the continued explicit declaration.)
//
// Author: james.synge@gmail.com

#include "utils/platform.h"
#include "utils/printable_progmem_string.h"

#if TAS_HOST_TARGET
#include <ostream>  // pragma: keep standard include
#endif

#define TASENUMERATOR(id, text) id

namespace alpaca {

// At a high-level, what is going on with the decoder?
enum class RequestDecoderStatus : uint8_t {
  kReset,
  kDecoding,
  kDecoded,
};

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

  // NOTE: Each values in the 4xx and 5xx range should have a corresponding
  // Literal in literals.inc, and a case in the switch statement in
  // WriteResponse::HttpErrorResponse.
  //
  // TODO(jamessynge): Consider adding an 'error code space' to Status so that
  // we can support two or three spaces: ASCOM error codes, HTTP status codes,
  // and possibly Unix errno values. That could help avoid the need for the
  // switch statement in WriteResponse::HttpErrorResponse, which has the effect
  // of causing all of the error literals to be linked in to the binary if
  // HttpErrorResponse is also linked in. Instead we could have a separate
  // function for each HTTP status code, like those in ascom_error_codes.* for
  // ASCOM error codes.

  // Invalid syntax in the request.
  kHttpBadRequest = 400,

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

using EHttpMethod_UnderlyingType = uint8_t;
enum class EHttpMethod : EHttpMethod_UnderlyingType {
  kUnknown = 0,
  // The supported HTTP methods. Note that the the HTTP/1.1 standard requires
  // that the methods GET and HEAD are supported.
  GET = 1,
  PUT = 2,
  HEAD = 3,
};

using EApiGroup_UnderlyingType = uint8_t;
enum class EApiGroup : uint8_t {
  kUnknown = 0,
  kDevice,      // Path: /api...
  kManagement,  // Path: /management...
  kSetup,       // Path: /setup...
};

using EAlpacaApi_UnderlyingType = uint8_t;
enum class EAlpacaApi : uint8_t {
  kUnknown = 0,

  // Path: /api/v1/{device_type}/{device_number}/{method}
  kDeviceApi,

  // Path: /setup/v1/{device_type}/{device_number}/setup
  kDeviceSetup,

  // Path: /management/apiversions
  kManagementApiVersions,

  // Path: /management/v1/description
  kManagementDescription,

  // Path: /management/v1/configureddevices
  kManagementConfiguredDevices,

  // Path: /setup
  kServerSetup,
};

using EManagementMethod_UnderlyingType = uint8_t;
enum class EManagementMethod : EManagementMethod_UnderlyingType {
  kUnknown,
  kDescription,
  kConfiguredDevices,
};

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
  kAction,
  kCommandBlind,
  kCommandBool,
  kCommandString,
  kConnected,
  kDescription,
  kDriverInfo,
  kDriverVersion,
  kInterfaceVersion,
  kName,
  kSupportedActions,

  // CoverCalibrator methods:
  kBrightness,
  kCalibratorState,
  kCoverState,
  kMaxBrightness,

  kCalibratorOff,
  kCalibratorOn,
  kCloseCover,
  kHaltCover,
  kOpenCover,

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

  // Supported Switch methods:
  kMaxSwitch,
  kCanWrite,
  kGetSwitch,
  kGetSwitchDescription,
  kGetSwitchName,
  kGetSwitchValue,
  kMinSwitchValue,
  kMaxSwitchValue,
  kSetSwitch,
  kSetSwitchName,
  kSetSwitchValue,
  kSwitchStep,
};

// These are parameter names used in *requests*, not responses. Names such as
// ServerTransactionID and ErrorNumber should not be in this list.
using EParameter_UnderlyingType = uint8_t;
enum class EParameter : EParameter_UnderlyingType {
  kUnknown,

  // Common or all method parameters.
  kAction,
  kClientID,
  kClientTransactionID,
  kCommand,
  kConnected,
  kParameters,
  kRaw,

  // ObservingConditions parameters.
  kBrightness,
  kSensorName,

  // Switch parameters.
  kId,
  kName,  // DECODING NOT YET SUPPORTED (No provision for storage)
  kState,
  kValue,
};

// These are sensor names used in an ObservingConditions SensorDescription
// requests, e.g. DewPoint or SkyBrightness. These are to be matched case
// insensitively.
using ESensorName_UnderlyingType = uint8_t;
enum class ESensorName : ESensorName_UnderlyingType {
  kUnknown,

  kCloudCover,
  kDewPoint,
  kHumidity,
  kPressure,
  kRainRate,
  kSkyBrightness,
  kSkyQuality,
  kSkyTemperature,
  kStarFullWidthHalfMax,
  kTemperature,
  kWindDirection,
  kWindGust,
  kWindSpeed,
};

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

// This is used for generating responses, not for input.
enum class EContentType : uint8_t {
  TASENUMERATOR(kApplicationJson, "application/json"),
  TASENUMERATOR(kTextPlain, "text/plain"),
  TASENUMERATOR(kTextHtml, "text/html"),
};

const __FlashStringHelper* ToFlashStringHelper(RequestDecoderStatus v);
const __FlashStringHelper* ToFlashStringHelper(EHttpStatusCode v);
const __FlashStringHelper* ToFlashStringHelper(EHttpMethod v);
const __FlashStringHelper* ToFlashStringHelper(EApiGroup v);
const __FlashStringHelper* ToFlashStringHelper(EAlpacaApi v);
const __FlashStringHelper* ToFlashStringHelper(EManagementMethod v);
const __FlashStringHelper* ToFlashStringHelper(EDeviceType v);
const __FlashStringHelper* ToFlashStringHelper(EDeviceMethod v);
const __FlashStringHelper* ToFlashStringHelper(EParameter v);
const __FlashStringHelper* ToFlashStringHelper(ESensorName v);
const __FlashStringHelper* ToFlashStringHelper(EHttpHeader v);
const __FlashStringHelper* ToFlashStringHelper(EContentType v);

size_t PrintValueTo(RequestDecoderStatus v, Print& out);
size_t PrintValueTo(EHttpStatusCode v, Print& out);
size_t PrintValueTo(EHttpMethod v, Print& out);
size_t PrintValueTo(EApiGroup v, Print& out);
size_t PrintValueTo(EAlpacaApi v, Print& out);
size_t PrintValueTo(EManagementMethod v, Print& out);
size_t PrintValueTo(EDeviceType v, Print& out);
size_t PrintValueTo(EDeviceMethod v, Print& out);
size_t PrintValueTo(EParameter v, Print& out);
size_t PrintValueTo(ESensorName v, Print& out);
size_t PrintValueTo(EHttpHeader v, Print& out);
size_t PrintValueTo(EContentType v, Print& out);

#if TAS_HOST_TARGET
// Support for debug logging of enums.
std::ostream& operator<<(std::ostream& os, RequestDecoderStatus v);
std::ostream& operator<<(std::ostream& os, EHttpStatusCode v);
std::ostream& operator<<(std::ostream& os, EHttpMethod v);
std::ostream& operator<<(std::ostream& os, EApiGroup v);
std::ostream& operator<<(std::ostream& os, EAlpacaApi v);
std::ostream& operator<<(std::ostream& os, EManagementMethod v);
std::ostream& operator<<(std::ostream& os, EDeviceType v);
std::ostream& operator<<(std::ostream& os, EDeviceMethod v);
std::ostream& operator<<(std::ostream& os, EParameter v);
std::ostream& operator<<(std::ostream& os, ESensorName v);
std::ostream& operator<<(std::ostream& os, EHttpHeader v);
std::ostream& operator<<(std::ostream& os, EContentType v);
#endif  // TAS_HOST_TARGET
}  // namespace alpaca

#undef TASENUMERATOR

#endif  // TINY_ALPACA_SERVER_SRC_CONSTANTS_H_
