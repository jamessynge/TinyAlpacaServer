#include "request_decoder_listener.h"

// Author: james.synge@gmail.com

#include "constants.h"
#include "utils/logging.h"
#include "utils/string_view.h"

namespace alpaca {

RequestDecoderListener::~RequestDecoderListener() {}

EHttpStatusCode RequestDecoderListener::OnExtraParameter(
    EParameter param, const StringView& value) {
  TAS_DVLOG(1, "OnExtraParameter(" << param << ", " << ToHexEscapedString(value)
                                   << ")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnExtraHeader(EHttpHeader header,
                                                      const StringView& value) {
  TAS_DVLOG(1, "OnExtraHeader(" << header << ", " << ToHexEscapedString(value)
                                << ")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterName(
    const StringView& name) {
  TAS_DVLOG(1, "OnUnknownParameterName(" << ToHexEscapedString(name) << ")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownParameterValue(
    const StringView& value) {
  TAS_DVLOG(1, "OnUnknownParameterValue(" << ToHexEscapedString(value) << ")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderName(
    const StringView& name) {
  TAS_DVLOG(1, "OnUnknownHeaderName(" << ToHexEscapedString(name) << ")");
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderListener::OnUnknownHeaderValue(
    const StringView& value) {
  TAS_DVLOG(1, "OnUnknownHeaderValue(" << ToHexEscapedString(value) << ")");
  return EHttpStatusCode::kContinueDecoding;
}

}  // namespace alpaca
