#include "request_decoder_listener.h"

// Author: james.synge@gmail.com

#include "constants.h"
#include "hex_escape.h"
#include "logging.h"
#include "string_view.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EHttpStatusCode RequestDecoderListener::OnExtraParameter(
    EParameter param, const mcucore::StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnExtraParameter(") << param
              << TAS_FLASHSTR(", ") << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(
    EHttpHeader header, const mcucore::StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnExtraHeader(") << header << TAS_FLASHSTR(", ")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const mcucore::StringView& name) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownParameterName(")
              << mcucore::HexEscaped(name) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const mcucore::StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownParameterValue(")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const mcucore::StringView& name) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownHeaderName(")
              << mcucore::HexEscaped(name) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const mcucore::StringView& value) {
  TAS_VLOG(1) << TAS_FLASHSTR("OnUnknownHeaderValue(")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
