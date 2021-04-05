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
  TAS_VLOG(1) << TASLIT("OnExtraParameter(") << param << TASLIT(", ")
              << HexEscaped(value) << TASLIT(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                      const StringView& value) {
  TAS_VLOG(1) << TASLIT("OnExtraHeader(") << header << TASLIT(", ")
              << HexEscaped(value) << TASLIT(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  TAS_VLOG(1) << TASLIT("OnUnknownParameterName(") << HexEscaped(name)
              << TASLIT(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  TAS_VLOG(1) << TASLIT("OnUnknownParameterValue(") << HexEscaped(value)
              << TASLIT(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  TAS_VLOG(1) << TASLIT("OnUnknownHeaderName(") << HexEscaped(name)
              << TASLIT(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  TAS_VLOG(1) << TASLIT("OnUnknownHeaderValue(") << HexEscaped(value)
              << TASLIT(")");
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
