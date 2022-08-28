#include "request_decoder_listener.h"

// Author: james.synge@gmail.com

#include <McuCore.h>

#include "constants.h"

namespace alpaca {

#if TAS_ENABLE_REQUEST_DECODER_LISTENER
RequestDecoderListener::~RequestDecoderListener() {}
#endif

#if TAS_ENABLE_ASSET_PATH_DECODING
EHttpStatusCode RequestDecoderListener::OnAssetPathSegment(
    const mcucore::StringView& segment) {
  MCU_VLOG(1) << MCU_PSD("OnAssetPathSegment(") << mcucore::HexEscaped(segment)
              << ')';
  return EHttpStatusCode::kContinueDecoding;
}
#endif  // TAS_ENABLE_ASSET_PATH_DECODING

#if TAS_ENABLE_EXTRA_PARAMETER_DECODING
EHttpStatusCode RequestDecoderListener::OnExtraParameter(
    EParameter param, const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_PSD("OnExtraParameter(") << param << MCU_PSD(", ")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}
#endif  // TAS_ENABLE_EXTRA_PARAMETER_DECODING

#if TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
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
#endif  // TAS_ENABLE_UNKNOWN_PARAMETER_DECODING

#if TAS_ENABLE_EXTRA_HEADER_DECODING
EHttpStatusCode RequestDecoderListener::OnExtraHeader(
    EHttpHeader header, const mcucore::StringView& value) {
  MCU_VLOG(1) << MCU_PSD("OnExtraHeader(") << header << MCU_PSD(", ")
              << mcucore::HexEscaped(value) << ')';
  return EHttpStatusCode::kContinueDecoding;
}
#endif  // TAS_ENABLE_EXTRA_HEADER_DECODING

#if TAS_ENABLE_UNKNOWN_HEADER_DECODING
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
#endif  // TAS_ENABLE_UNKNOWN_HEADER_DECODING

}  // namespace alpaca
