#include "request_decoder_listener.h"

// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EHttpStatusCode RequestDecoderListener::OnExtraParameter(
    EParameter param, const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_PSD("OnExtraParameter(") << param << MCU_PSD(", ")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(
    EHttpHeader header, const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_PSD("OnExtraHeader(") << header << MCU_PSD(", ")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const mcucore::StringView& name) {
  MCU_VLOG(1) << MCU_PSD("OnUnknownParameterName(") << mcucore::HexEscaped(name)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_PSD("OnUnknownParameterValue(")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const mcucore::StringView& name) {
  MCU_VLOG(1) << MCU_PSD("OnUnknownHeaderName(") << mcucore::HexEscaped(name)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_PSD("OnUnknownHeaderValue(") << mcucore::HexEscaped(value)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
