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
  TAS_VLOG(1) << FLASHSTR("OnExtraParameter(") << param << FLASHSTR(", ")
              << HexEscaped(value) << FLASHSTR(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                      const StringView& value) {
  TAS_VLOG(1) << FLASHSTR("OnExtraHeader(") << header << FLASHSTR(", ")
              << HexEscaped(value) << FLASHSTR(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  TAS_VLOG(1) << FLASHSTR("OnUnknownParameterName(") << HexEscaped(name)
              << FLASHSTR(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  TAS_VLOG(1) << FLASHSTR("OnUnknownParameterValue(") << HexEscaped(value)
              << FLASHSTR(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  TAS_VLOG(1) << FLASHSTR("OnUnknownHeaderName(") << HexEscaped(name)
              << FLASHSTR(")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  TAS_VLOG(1) << FLASHSTR("OnUnknownHeaderValue(") << HexEscaped(value)
              << FLASHSTR(")");
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
