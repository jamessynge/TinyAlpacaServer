#ifndef TINY_ALPACA_SERVER_SRC_CONSTANTS_H_
#define TINY_ALPACA_SERVER_SRC_CONSTANTS_H_

// Enums defining the HTTP status and enums corresponding to each kind of token
// to be decoded, or state which must be shared across libraries in Tiny Alpaca
// Server.
//
// In addition, make_enum_to_string.py is used to generate functions that which
// support printing an enumerators name given its value. Specifically:
//
// *  ToFlashStringHelper returns a pointer to a string stored in flash given a
//    valid enumerator, else returns a nullptr value is not a known enumerator.
//
// *  PrintValueTo prints the name returned by ToFlashStringHelper to a Print
//    instance; if instead a nullptr was returned, a message is printed
//    indicating the enum type and the raw value of the enumerator.
//
// In support of testing, the generated code also includes std::ostream
// insertion operators. These should not (and really, cannot) be used in the
// embedded code as Arduino libraries don't include a remotely complete C++
// standard library.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#if MCU_HOST_TARGET
// Must come after mcucore_platform.h so that MCU_HOST_TARGET is defined.
#include <ostream>  // pragma: keep standard include
#endif

#define TASENUMERATOR(id, text) id

namespace alpaca {

// At a high-level, what is going on with the decoder?
enum class RequestDecoderStatus : uint_fast8_t {
  kReset,
  kDecoding,
  kDecoded,
};

// Most of these are based on HTTP Response mcucore::Status Codes, the others
// are below 100 (HTTP's continue status).
enum class EHttpStatusCode : uint_fast16_t {
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
  TASENUMERATOR(kHttpOk, "OK") = 200,

  // NOTE: Each values in the 4xx and 5xx range should have a corresponding
  // mcucore::ProgmemStringView in literals.inc, and a case in the switch
  // statement in
  // WriteResponse::HttpErrorResponse.
  //
  // TODO(jamessynge): Consider adding an 'error code space' to mcucore::Status
  // so that we can support two or three spaces: ASCOM error codes, HTTP status
  // codes, and possibly Unix errno values. That could help avoid the need for
  // the switch statement in WriteResponse::HttpErrorResponse, which has the
  // effect of causing all of the error literals to be linked in to the binary
  // if HttpErrorResponse is also linked in. Instead we could have a separate
  // function for each HTTP status code, like those in ascom_error_codes.* for
  // ASCOM error codes.

  // Invalid syntax in the request.
  TASENUMERATOR(kHttpBadRequest, "Bad Request") = 400,

  // Not a supported/known path.
  TASENUMERATOR(kHttpNotFound, "Not Found") = 404,

  // Unknown/unsupported HTTP method.
  TASENUMERATOR(kHttpMethodNotAllowed, "Method Not Allowed") = 405,

  // If Accept header from client doesn't include application/json.
  TASENUMERATOR(kHttpNotAcceptable, "Not Acceptable") = 406,

  // Need the length to know where the end of the body (payload) is; we don't
  // support encodings that require delimiters to be located.
  TASENUMERATOR(kHttpLengthRequired, "Length Required") = 411,

  // Either the client sent more body bytes than the Content-Length specified
  // (maybe pipelining requests, which isn't supported) or the Content-Length
  // was greater than the maximum supported value.
  TASENUMERATOR(kHttpPayloadTooLarge, "Payload Too Large") = 413,

  // The Content-Type header didn't specify a supported value, which is
  // currently only application/x-www-form-urlencoded.
  TASENUMERATOR(kHttpUnsupportedMediaType, "Unsupported Media Type") = 415,

  // May use for extra-long names and values.
  TASENUMERATOR(kHttpRequestHeaderFieldsTooLarge,
                "Request Header Fields Too Large") = 431,

  // Unspecified problem with processing the request.
  TASENUMERATOR(kHttpInternalServerError, "Internal Server Error") = 500,

  // We only support GET, HEAD and PUT, and return this for any other method.
  TASENUMERATOR(kHttpNotImplemented, "Not Implemented") = 501,

  // Only HTTP/1.1 is supported. Could support 1.0 easily enough.
  TASENUMERATOR(kHttpVersionNotSupported, "HTTP Version Not Supported") = 505,
};

enum class EHttpMethod : uint_fast8_t {
  kUnknown = 0,
  // The supported HTTP methods. Note that the the HTTP/1.1 standard requires
  // that the methods GET and HEAD are supported.
  GET = 1,
  PUT = 2,
  HEAD = 3,
};

enum class EApiGroup : uint_fast8_t {
  kUnknown = 0,
  kDevice,        // Path: /api...
  kManagement,    // Path: /management...
  kSetup,         // Path: /setup...
  kAsset,         // Path: /asset...
  kServerStatus,  // Path: /
};

enum class EAlpacaApi : uint_fast8_t {
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

  // Path: /asset/...
  kAsset,

  // Path: /setup
  kServerSetup,

  // Path: /
  kServerStatus,
};

enum class EManagementMethod : uint_fast8_t {
  kUnknown,
  kDescription,
  kConfiguredDevices,
};

// Note that we depend in the HTML generation code on the generated PrintValueTo
// for EDeviceType to produce a value that can be used as part of an HTML
// element's class name, which must not have any spaces in it. Thus you should
// not use TASENUMERATOR here to provide an alternate string
enum class EDeviceType : uint_fast8_t {
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

// Note that for many PUT methods, the name of the method must lower case in the
// request path (e.g. "averageperiod"), but that same name is used as a
// case-insensitive parameter same in the body of the request.
enum class EDeviceMethod : uint_fast8_t {
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
  kStarFWHM,
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
enum class EParameter : uint_fast8_t {
  kUnknown,

  // Common or all method parameters.
  kAction,
  kClientID,
  kClientTransactionID,
  kCommand,
  kConnected,
  kParameters,
  kRaw,

  // Calibrator parameters.
  kBrightness,

  // ObservingConditions parameters.
  kAveragePeriod,
  kSensorName,

  // Switch parameters.
  kId,
  kName,
  kState,
  kValue,
};

// These are sensor names used in an ObservingConditions SensorDescription
// requests, e.g. DewPoint or SkyBrightness. These are to be matched case
// insensitively.
enum class ESensorName : uint_fast8_t {
  kUnknown,

  kCloudCover,
  kDewPoint,
  kHumidity,
  kPressure,
  kRainRate,
  kSkyBrightness,
  kSkyQuality,
  kSkyTemperature,
  kStarFWHM,
  kTemperature,
  kWindDirection,
  kWindGust,
  kWindSpeed,
};

enum class EHttpHeader : uint_fast8_t {
  kUnknown,

  kConnection,
  TASENUMERATOR(kContentLength, "Content-Length"),
  TASENUMERATOR(kContentType, "Content-Type"),

  // Date is used in tests as an example of a header whose name we know but for
  // which there is not built-in decoding, therefore it may be handled by
  // RequestDecoderListener::OnExtraHeader. Since this isn't a header that a
  // client should send to a server, it is a good choice because we won't need
  // try to fit the entire value of a Date header's value into buffers for
  // passing to RequestDecoder because we won't receive it.
  TASENUMERATOR(kDate, "Date"),
};

// This is used for generating responses, not for input.
enum class EContentType : uint_fast8_t {
  TASENUMERATOR(kApplicationJson, "application/json"),
  TASENUMERATOR(kTextPlain, "text/plain"),
  TASENUMERATOR(kTextHtml, "text/html"),
};

// This is used for generating HTML responses, not for input.
enum class EHtmlPageSection : uint_fast8_t {
  kHead,     // Inside the <head></head>
  kBody,     // Inside the <body></body>
  kTrailer,  // End of the body, before </body>
};

// Tag numbers for EEPROM entries.
enum class EDeviceEepromTagId : uint8_t {
  kUniqueId = 1,
};

#undef TASENUMERATOR

}  // namespace alpaca

// BEGIN_HEADER_GENERATED_BY_MAKE_ENUM_TO_STRING

namespace alpaca {

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
const __FlashStringHelper* ToFlashStringHelper(EHtmlPageSection v);
const __FlashStringHelper* ToFlashStringHelper(EDeviceEepromTagId v);

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
size_t PrintValueTo(EHtmlPageSection v, Print& out);
size_t PrintValueTo(EDeviceEepromTagId v, Print& out);

#if MCU_HOST_TARGET
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
std::ostream& operator<<(std::ostream& os, EHtmlPageSection v);
std::ostream& operator<<(std::ostream& os, EDeviceEepromTagId v);
#endif  // MCU_HOST_TARGET

}  // namespace alpaca

// END_HEADER_GENERATED_BY_MAKE_ENUM_TO_STRING

#endif  // TINY_ALPACA_SERVER_SRC_CONSTANTS_H_
