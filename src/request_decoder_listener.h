#ifndef TINY_ALPACA_SERVER_SRC_REQUEST_DECODER_LISTENER_H_
#define TINY_ALPACA_SERVER_SRC_REQUEST_DECODER_LISTENER_H_

// Listener for events regarding unrecognized or unsupported query and body
// parameters, and unrecognized or unsupported  headers.
//
// Author: james.synge@gmail.com

#include "decoder_constants.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {

class RequestDecoderListener {
 public:
  virtual ~RequestDecoderListener();

  // Called to handle recognized parameters for which there isn't built in
  // decoding; i.e. ClientId is decoded and stored by the decoder, but
  // AveragePeriod, for example, doesn't have built in support; however if we
  // add AveragePeriod to kRecognizedParameters in tokens.h, then this method
  // will be called if the client includes that parameter in the request.
  // Also used if a parameter with built-in support is invalid (e.g. can't be
  // converted to an integer, or is a duplicate). It is then up to the listener
  // to decide on the status (e.g. kHttpBadRequest or kContinueDecoding).
  //
  // If this method returns kContinueDecoding, decoding continues. Any other
  // value is interpreted as an error, though the value should be an HTTP
  // Response Status Code, not an enum whose underlying value is below 400
  // kNeedMoreInput or kHttpOk(those are converted to kHttpInternalServerError).
  virtual EDecodeStatus OnExtraParameter(EParameter param,
                                         const StringView& value);

  // As above, but for recognized but not directly supported EHttpHeader values.
  virtual EDecodeStatus OnExtraHeader(EHttpHeader header,
                                      const StringView& value);

  // Called to handle unrecognized parameters, with one call for the name and
  // another for the value. There is no guarantee that the name will still be
  // in the underlying buffer when the value method is called.
  // The return value is treated as described above.
  virtual EDecodeStatus OnUnknownParameterName(const StringView& name);
  virtual EDecodeStatus OnUnknownParameterValue(const StringView& value);

  // As above, but for unrecognized headers.
  virtual EDecodeStatus OnUnknownHeaderName(const StringView& name);
  virtual EDecodeStatus OnUnknownHeaderValue(const StringView& value);
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_REQUEST_DECODER_LISTENER_H_
