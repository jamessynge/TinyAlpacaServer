#include "request_decoder_listener.h"

// Author: james.synge@gmail.com

#include "constants.h"
#include "utils/hex_escape.h"
#include "utils/logging.h"
#include "utils/string_view.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EHttpStatusCode RequestDecoderListener::OnExtraParameter(
    EParameter param, const StringView& value) {
  TAS_VLOG(1) << "OnExtraParameter(" << param << ", " << HexEscaped(value)
              << ")";
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                      const StringView& value) {
  TAS_VLOG(1) << "OnExtraHeader(" << header << ", " << HexEscaped(value) << ")";
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  TAS_VLOG(1) << "OnUnknownParameterName(" << HexEscaped(name) << ")";
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  TAS_VLOG(1) << "OnUnknownParameterValue(" << HexEscaped(value) << ")";
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  TAS_VLOG(1) << "OnUnknownHeaderName(" << HexEscaped(name) << ")";
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  TAS_VLOG(1) << "OnUnknownHeaderValue(" << HexEscaped(value) << ")";
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
