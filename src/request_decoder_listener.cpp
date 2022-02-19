#include "request_decoder_listener.h"

// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EHttpStatusCode RequestDecoderListener::OnExtraParameter(
    EParameter param, const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_FLASHSTR("OnExtraParameter(") << param
              << MCU_FLASHSTR(", ") << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(
    EHttpHeader header, const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_FLASHSTR("OnExtraHeader(") << header << MCU_FLASHSTR(", ")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const mcucore::StringView& name) {
  MCU_VLOG(1) << MCU_FLASHSTR("OnUnknownParameterName(")
              << mcucore::HexEscaped(name) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_FLASHSTR("OnUnknownParameterValue(")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const mcucore::StringView& name) {
  MCU_VLOG(1) << MCU_FLASHSTR("OnUnknownHeaderName(")
              << mcucore::HexEscaped(name) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_FLASHSTR("OnUnknownHeaderValue(")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
