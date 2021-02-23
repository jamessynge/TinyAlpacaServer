#include "src/request_decoder_listener.h"

// Author: james.synge@gmail.com

#include "src/decoder_constants.h"
#include "src/logging.h"
#include "src/string_view.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EDecodeStatus RequestDecoderListener::OnExtraParameter(
    EParameter param, const StringView& value) {
  TAS_DVLOG(1, "OnExtraParameter(" << param << ", "
                                   << value.ToHexEscapedString() << ")");
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                    const StringView& value) {
  TAS_DVLOG(1, "OnExtraHeader(" << header << ", " << value.ToHexEscapedString()
                                << ")");
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  TAS_DVLOG(1, "OnUnknownParameterName(" << name.ToHexEscapedString() << ")");
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  TAS_DVLOG(1, "OnUnknownParameterValue(" << value.ToHexEscapedString() << ")");
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  TAS_DVLOG(1, "OnUnknownHeaderName(" << name.ToHexEscapedString() << ")");
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  TAS_DVLOG(1, "OnUnknownHeaderValue(" << value.ToHexEscapedString() << ")");
  return EDecodeStatus::kContinueDecoding;
}

}  // namespace alpaca
