#include "alpaca-decoder/request_decoder_listener.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EDecodeStatus RequestDecoderListener::OnExtraParameter(
    EParameter param, const StringView& value) {
  DVLOG(1) << "OnExtraParameter(" << param << ", " << value.ToEscapedString()
           << ")";
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                    const StringView& value) {
  DVLOG(1) << "OnExtraHeader(" << header << ", " << value.ToEscapedString()
           << ")";
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  DVLOG(1) << "OnUnknownParameterName(" << name.ToEscapedString() << ")";
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  DVLOG(1) << "OnUnknownParameterValue(" << value.ToEscapedString() << ")";
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  DVLOG(1) << "OnUnknownHeaderName(" << name.ToEscapedString() << ")";
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  DVLOG(1) << "OnUnknownHeaderValue(" << value.ToEscapedString() << ")";
  return EDecodeStatus::kContinueDecoding;
}

}  // namespace alpaca
