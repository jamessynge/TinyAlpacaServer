#ifndef TINY_ALPACA_SERVER_SRC_REQUEST_DECODER_LISTENER_H_
#define TINY_ALPACA_SERVER_SRC_REQUEST_DECODER_LISTENER_H_

// Listener for events regarding unrecognized or unsupported query and body
// parameters, and unrecognized or unsupported  headers. The class is defined
// IFF one of the features it supports is enabled.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "config.h"  // IWYU pragma: keep
#include "constants.h"

// Set TAS_ENABLE_REQUEST_DECODER_LISTENER to non-zero if any of the features it
// supports are enabled.
#if TAS_ENABLE_ASSET_PATH_DECODING || TAS_ENABLE_EXTRA_PARAMETER_DECODING || \
    TAS_ENABLE_UNKNOWN_PARAMETER_DECODING ||                                 \
    TAS_ENABLE_EXTRA_HEADER_DECODING || TAS_ENABLE_UNKNOWN_HEADER_DECODING
#define TAS_ENABLE_REQUEST_DECODER_LISTENER 1
#else
#define TAS_ENABLE_REQUEST_DECODER_LISTENER 0
#endif

namespace alpaca {

#if TAS_ENABLE_REQUEST_DECODER_LISTENER

class RequestDecoderListener {
 public:
  virtual ~RequestDecoderListener();

#if TAS_ENABLE_ASSET_PATH_DECODING
  // If the path starts /asset/, then subsequent segments of the path are passed
  // to this method. is_last==true only for the last segment of the path; and if
  // the path ends with a slash, segment will be empty.
  virtual EHttpStatusCode OnAssetPathSegment(const mcucore::StringView& segment,
                                             bool is_last_segment);
#endif  // TAS_ENABLE_ASSET_PATH_DECODING

#if TAS_ENABLE_EXTRA_PARAMETER_DECODING
  // Called to handle recognized parameters for which there isn't built in
  // decoding, and when the value of the parameter is invalid (e.g. if the
  // SensorName parameter is provided, but the value is missing or is
  // unrecognized). For example, ClientID is fully supported, so it is decoded
  // and stored by the decoder, but AveragePeriod doesn't have built in support;
  // however if we add AveragePeriod to kRecognizedParameters in tokens.h, then
  // this method will be called if the client includes that parameter in the
  // request.
  //
  // If the parameter value is known to be invalid by the decoder, the return
  // value is ignored if it is not an error code. Otherwise, if this method
  // returns kContinueDecoding, decoding continues. Any other value is
  // interpreted as an error, though the value should be an HTTP Response
  // mcucore::Status Code, not an enum whose underlying value is below 400
  // kNeedMoreInput or kHttpOk(those are converted to kHttpInternalServerError).
  virtual EHttpStatusCode OnExtraParameter(EParameter param,
                                           const mcucore::StringView& value);
#endif  // TAS_ENABLE_EXTRA_PARAMETER_DECODING

#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
  // Called to handle unrecognized parameters, with one call for the name and
  // another for the value. There is no guarantee that the name will still be
  // in the underlying buffer when the value method is called.
  // The return value is treated as described above.
  virtual EHttpStatusCode OnUnknownParameterName(
      const mcucore::StringView& name);
  virtual EHttpStatusCode OnUnknownParameterValue(
      const mcucore::StringView& value);
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING

#if TAS_ENABLE_EXTRA_HEADER_DECODING
  // Like OnExtraParameter, but for recognized but not directly supported
  // EHttpHeader values, or second appearances of the same supported header (an
  // error).
  virtual EHttpStatusCode OnExtraHeader(EHttpHeader header,
                                        const mcucore::StringView& value);
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING

#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
  // Like the OnUnknownParameter* methods, but for unrecognized headers.
  virtual EHttpStatusCode OnUnknownHeaderName(const mcucore::StringView& name);
  virtual EHttpStatusCode OnUnknownHeaderValue(
      const mcucore::StringView& value);
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING
};

#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_REQUEST_DECODER_LISTENER_H_
