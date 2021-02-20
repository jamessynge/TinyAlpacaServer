#ifndef TINY_ALPACA_SERVER_REQUEST_DECODER_H_
#define TINY_ALPACA_SERVER_REQUEST_DECODER_H_

// RequestDecoder is a (fairly) strict HTTP/1.1 Request Message decoder
// targetted at the requests defined for the ASCOM Alpaca REST API.

// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "decoder_constants.h"
#include "request_decoder_listener.h"
#include "string_view.h"

namespace alpaca {

// CONSIDER: We could add a listener for errors only so that we could produce
// an error message incrementally (i.e. StartError, AddErrorMessage, EndError),
// thus allowing the caller to incrementally create and send fragments of an
// HTTP error message, thus not needing a large buffer.

struct RequestDecoderState {
  using DecodeFunction = EDecodeStatus (*)(RequestDecoderState&, StringView&);

  RequestDecoderState(AlpacaRequest& request, RequestDecoderListener& listener);

  // Prepares for decoding a new request
  void Reset();

  // Repeatedly applies the current decode function to the input until done,
  // needs more input than is in buffer, or an error is detected.
  EDecodeStatus DecodeBuffer(StringView& buffer, bool buffer_is_full,
                             bool at_end_of_input);

  // Set the function to be used for decoding the leading edge of the input.
  // Returns kParseInProgress.
  EDecodeStatus SetDecodeFunction(DecodeFunction func);

  EDecodeStatus SetDecodeFunctionAfterListenerCall(DecodeFunction func,
                                                   EDecodeStatus status);

 private:
  // Apply decode_function just once, compute new status.
  // EDecodeStatus DecodeBufferAtEnd(StringView& buffer);
  // EDecodeStatus ApplyDecodeFunction(StringView& buffer);
  EDecodeStatus DecodeMessageHeader(StringView& buffer, bool at_end_of_input);
  EDecodeStatus DecodeMessageBody(StringView& buffer, bool at_end_of_input);

 public:
  DecodeFunction decode_function;

  union {
    EParameter current_parameter;
    EHttpHeader current_header;
  };
  // NOTE: We could change this to uint16_t if we need to support longer
  // payloads.
  StringView::size_type remaining_content_length;

  // Using bit fields here for these boolean values, which represents a
  // trade-off of program size for smaller RAM use. Will need to assess if this
  // makes sense.
  unsigned int is_decoding_header : 1;
  unsigned int is_decoding_start_line : 1;
  unsigned int is_final_input : 1;
  unsigned int found_content_length : 1;

  AlpacaRequest& request;
  RequestDecoderListener& listener;
};

// Supports decoding the request headers of HTTP messages (one at a time).
class RequestDecoder : RequestDecoderState {
 public:
  using RequestDecoderState::DecodeBuffer;
  using RequestDecoderState::RequestDecoderState;
  using RequestDecoderState::Reset;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_REQUEST_DECODER_H_
