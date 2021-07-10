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
  TAS_VLOG(1) << TAS_FLASHSTR("OnExtraParameter(") << param
              << TAS_FLASHSTR(", ") << HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                      const StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnExtraHeader(") << header << TAS_FLASHSTR(", ")
              << HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownParameterName(") << HexEscaped(name)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownParameterValue(") << HexEscaped(value)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownHeaderName(") << HexEscaped(name)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownHeaderValue(") << HexEscaped(value)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
