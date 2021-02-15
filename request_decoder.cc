// Implementation of an HTTP Request Decoder for the ASCOM Alpaca REST API.
// Author: james.synge@gmail.com

// TODO(jamessynge): Consider whether to just return 400 Bad Request for almost
// all problems with parsing, rather than includiing "elaborate" efforts to
// validate the input. One choice is to add a macro guard (i.e. for #ifdef
// blocks) so that we can enable the features when there is enough code space,
// but omit when there isn't.
//
// Another way to reduce the size of *this* code is to pass the buck to the
// calling code (i.e. via the listener). The only Header that we "need" to
// decode is Content-Length.

#include "alpaca-decoder/request_decoder.h"

#include <cstddef>
#include <limits>

#include "absl/strings/ascii.h"
#include "alpaca-decoder/constants.h"
#include "alpaca-decoder/string_view.h"
#include "alpaca-decoder/token.h"
#include "alpaca-decoder/tokens.h"

// NOTE: The syntax for the query path is not as clearly specified as the rest
// of HTTP (AFAICT), so I'm assuming that:
// 1) A name is composed of upper and/or lower case ASCII letters (because those
//    are used by ASCOM Alpaca);
// 2) A value is any non-control character value, excluding space.
// 3) A name is followed by '=' and then by a value.
// 4) A name may not be empty, but some values may be empty; for example,
//    ClientId must be specified in order for this decoder to translate from
//    string to int, but some other parameters have their semantics provided by
//    the calling program, so we defer validation to that calling program
// 5) Percent encoded characters in values are not detected and decoded by this
//    decoder, and are just treated as regular characters. If they're in values
//    that must be explicitly matched or decoded by this decoder, that operation
//    is likely to fail.

namespace alpaca {
namespace {
using DecodeFunction = RequestDecoderState::DecodeFunction;
using CharMatchFunction = bool (*)(char c);

ALPACA_CONSTEXPR_VAR StringView kEndOfHeaderLine("\r\n");
ALPACA_CONSTEXPR_VAR StringView kHttp_1_1_EOL("HTTP/1.1\r\n");
ALPACA_CONSTEXPR_VAR StringView kAscomPathPrefix(" /api/v1/");

////////////////////////////////////////////////////////////////////////////////
// Helpers for decoder functions.

// QUESTION: Do these take up less space than including in cctype and calling
// std::isdigit, etc. They use some tables which take space, but that becomes
// moot if other code uses those same tables.
// bool IsDigit(const char c) { return '0' <= c && c <= '9'; }
// bool IsLower(const char c) { return 'a' <= c && c <= 'z'; }
// bool IsUpper(const char c) { return 'A' <= c && c <= 'Z'; }

bool IsVisibleCharOrSpace(const char c) { return ' ' <= c && c <= '~'; }

ALPACA_CONSTEXPR_VAR StringView kUnreservedNonAlnum("-._~");

bool IsUnreserved(const char c) {
  return absl::ascii_isalnum(c) || kUnreservedNonAlnum.contains(c);
}

// Match characters in either a URI query param or a header name; actually, just
// the subset of such characters we need to match for ASCOM Alpaca. Since we
// compare matching strings against tokens to find those we're interested in,
// having this set contain extra characters for some context doesn't really
// matter.
ALPACA_CONSTEXPR_VAR StringView kExtraNameChars("-_");
bool IsNameChar(const char c) {
  return absl::ascii_isalnum(c) || kExtraNameChars.contains(c);
}

// Match characters allowed in a URL encoded parameter value, whether in the
// path or in the body of a PUT request.
ALPACA_CONSTEXPR_VAR StringView kExtraParamValueChars("-_=%");
bool IsParamValueChar(const char c) {
  return absl::ascii_isalnum(c) || kExtraParamValueChars.contains(c);
}

StringView::size_type FindFirstNotOf(const StringView& view,
                                     bool (*test)(char)) {
  for (StringView::size_type pos = 0; pos < view.size(); ++pos) {
    if (!test(view.at(pos))) {
      return pos;
    }
  }
  return StringView::kMaxSize;
}

// Find the token in tokens that matches the view.
template <typename E, int N>
E MatchTokensExactly(const StringView& view, E unknown_id,
                     const Token<E> (&tokens)[N]) {
  for (int i = 0; i < N; ++i) {
    if (tokens[i].str == view) {
      DVLOG(3) << "MatchTokensExactly matched "
               << tokens[i].str.ToEscapedString() << " to "
               << view.ToEscapedString() << ", returning " << tokens[i].id;
      return tokens[i].id;
    }
  }
  DVLOG(3) << "MatchTokensExactly unable to match " << view.ToEscapedString()
           << ", returning '" << unknown_id;
  return unknown_id;
}

template <typename E, int N>
E LowerMatchTokens(const StringView& view, E unknown_id,
                   const Token<E> (&tokens)[N]) {
  for (int i = 0; i < N; ++i) {
    if (tokens[i].str.equals_other_lowered(view)) {
      DVLOG(3) << "LowerMatchTokens matched " << tokens[i].str.ToEscapedString()
               << " to " << view.ToEscapedString() << ", returning "
               << tokens[i].id;
      return tokens[i].id;
    }
  }
  DVLOG(3) << "LowerMatchTokens unable to match " << view.ToEscapedString()
           << ", returning '" << unknown_id;
  return unknown_id;
}

bool ExtractMatchingPrefix(RequestDecoderState& state, StringView& view,
                           StringView& extracted_prefix,
                           CharMatchFunction char_matcher) {
  auto beyond = FindFirstNotOf(view, char_matcher);
  if (beyond == StringView::kMaxSize) {
    // if (!state.is_final_input) {
    return false;
    // }
    // beyond = view.size();
  }
  extracted_prefix = view.prefix(beyond);
  view.remove_prefix(beyond);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Decoder functions for different phases of decoding. Generally in reverse
// order to avoid forward declarations.

// Necessary forward declarations (whereever we have a cycle in the grammar).
EDecodeStatus DecodeParamName(RequestDecoderState& state, StringView& view);
EDecodeStatus DecodeHeaderLines(RequestDecoderState& state, StringView& view);

EDecodeStatus DecodeHeaderLineEnd(RequestDecoderState& state,
                                  StringView& view) {
  // We expect "\r\n" at the end of a header line.
  DVLOG(2) << "DecodeHeaderLineEnd " << view.ToEscapedString();
  if (view.match_and_consume(kEndOfHeaderLine)) {
    return state.SetDecodeFunction(DecodeHeaderLines);
  } else if (kEndOfHeaderLine.starts_with(view)) {
    // Need more input.
    return EDecodeStatus::kNeedMoreInput;
  } else {
    // The header line doesn't end where or as expected; perhaps the EOL
    // terminator isn't correct (e.g. a "\n" instead of a  "\r\n").
    return EDecodeStatus::kHttpBadRequest;
  }
}

EDecodeStatus DecodeHeaderValue(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeHeaderValue " << view.ToEscapedString();
  StringView value;
  if (!ExtractMatchingPrefix(state, view, value, IsVisibleCharOrSpace)) {
    return EDecodeStatus::kNeedMoreInput;
  }
  DVLOG(1) << "DecodeHeaderValue raw value: " << value.ToEscapedString();
  // Trim whitespace from the start and end of the header value.
  while (value.starts_with(' ')) {
    value.remove_prefix(1);
  }
  while (value.ends_with(' ')) {
    value.remove_suffix(1);
  }
  DVLOG(1) << "DecodeHeaderValue trimmed value: " << value.ToEscapedString();
  EDecodeStatus status = EDecodeStatus::kContinueDecoding;
  if (state.current_header == EHttpHeader::kHttpAccept) {
    // Not tracking whether there are multiple accept headers.
    //
    // This is not a very complete comparison (i.e. would also match
    // "xxapplication/json+xyz"), but probably sufficient for our purpose.
    if (!value.contains(StringView("application/json"))) {
      status = state.listener.OnExtraHeader(EHttpHeader::kHttpAccept, value);
    }
  } else if (state.current_header == EHttpHeader::kHttpContentLength &&
             state.request.http_method == EHttpMethod::PUT) {
    // Note, we "ignore" the content-length for GET; it doesn't matter if it
    // is bogus.
    uint32_t content_length = 0;
    constexpr auto kMaxContentLength =
        std::numeric_limits<decltype(state.remaining_content_length)>::max();
    if (state.found_content_length || !value.to_uint32(content_length) ||
        content_length > kMaxContentLength) {
      status =
          state.listener.OnExtraHeader(EHttpHeader::kHttpContentLength, value);
      if (status <= EDecodeStatus::kHttpOk) {
        if (content_length > 0) {
          // It's out of range for our decoder.
          status = EDecodeStatus::kHttpPayloadTooLarge;
        } else {
          status = EDecodeStatus::kHttpBadRequest;
        }
      }
    } else {
      state.remaining_content_length = content_length;
      state.found_content_length = true;
    }
  } else if (state.current_header == EHttpHeader::kHttpContentType) {
    if (state.request.http_method == EHttpMethod::PUT &&
        value != StringView("application/x-www-form-urlencoded")) {
      status =
          state.listener.OnExtraHeader(EHttpHeader::kHttpContentType, value);
      if (status <= EDecodeStatus::kHttpOk) {
        status = EDecodeStatus::kHttpUnsupportedMediaType;
      }
    }
  } else if (state.current_header == EHttpHeader::kUnknown) {
    status = state.listener.OnUnknownHeaderValue(value);
  } else {
    // Recognized but no built-in support.
    status = state.listener.OnExtraHeader(state.current_header, value);
  }
  return state.SetDecodeFunctionAfterListenerCall(DecodeHeaderLineEnd, status);
}

EDecodeStatus MatchHeaderNameValueSeparator(RequestDecoderState& state,
                                            StringView& view) {
  DVLOG(2) << "MatchHeaderNameValueSeparator " << view.ToEscapedString();
  if (view.empty()) {
    return EDecodeStatus::kNeedMoreInput;
  } else if (view.at(0) != ':') {
    return EDecodeStatus::kHttpBadRequest;
  } else {
    view.remove_prefix(1);
    return state.SetDecodeFunction(DecodeHeaderValue);
  }
}

EDecodeStatus DecodeHeaderName(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeHeaderName " << view.ToEscapedString();
  StringView name;
  if (ExtractMatchingPrefix(state, view, name, IsNameChar)) {
    state.current_header =
        LowerMatchTokens(name, EHttpHeader::kUnknown, kRecognizedHttpHeaders);
    if (state.current_header == EHttpHeader::kUnknown) {
      return state.SetDecodeFunctionAfterListenerCall(
          MatchHeaderNameValueSeparator,
          state.listener.OnUnknownHeaderName(name));
    }
    return state.SetDecodeFunction(MatchHeaderNameValueSeparator);
  } else {
    return EDecodeStatus::kNeedMoreInput;
  }
}

EDecodeStatus DecodeHeaderLines(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeHeaderLines " << view.ToEscapedString();
  if (view.match_and_consume(kEndOfHeaderLine)) {
    // We've reached the end of the headers.
    if (state.request.http_method == EHttpMethod::GET) {
      // The standard requires that we not examine the body of a GET request,
      // if present, so we're done.
      return EDecodeStatus::kHttpOk;
    } else if (state.request.http_method != EHttpMethod::PUT) {
      // Shouldn't get here unless support for a new method is added to
      // DecodeHttpMethod, but not to here, or else if there is a bug.
      return EDecodeStatus::kInternalError;
    } else if (!state.found_content_length) {
      // We need to know the length in order to decode the body.
      return EDecodeStatus::kHttpLengthRequired;
    } else if (state.remaining_content_length == 0) {
      // Very odd, but it is possible that all of the parameters are in the
      // query parameters in the start line of the request. For example, the
      // "refresh" method of the "observingconditions" device type requires no
      // parameters.
      return EDecodeStatus::kHttpOk;
    } else {
      // There is a body of known length to be decoded.
      state.is_decoding_header = false;
      state.decode_function = DecodeParamName;
      return EDecodeStatus::kNeedMoreInput;
    }
  } else if (kEndOfHeaderLine.starts_with(view)) {
    // view might be empty, or it might be the start of a kEndOfHeaderLine.
    // To decide what to do next, we need more input.
    return EDecodeStatus::kNeedMoreInput;
  } else {
    return state.SetDecodeFunction(DecodeHeaderName);
  }
}

// An HTTP/1.1 Request Start Line should always end with "HTTP/1.1\r\n".
// (We're not supporting HTTP/1.0 or earlier.)
EDecodeStatus MatchHttpVersion(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "MatchHttpVersion " << view.ToEscapedString();
  if (view.match_and_consume(kHttp_1_1_EOL)) {
    state.is_decoding_start_line = false;
    return state.SetDecodeFunction(DecodeHeaderLines);
  } else if (kHttp_1_1_EOL.starts_with(view)) {
    return EDecodeStatus::kNeedMoreInput;
  } else {
    return EDecodeStatus::kHttpVersionNotSupported;
  }
}

EDecodeStatus DecodeParamSeparator(RequestDecoderState& state,
                                   StringView& view) {
  DVLOG(2) << "DecodeParamSeparator " << view.ToEscapedString();
  if (view.empty()) {
    if (!state.is_decoding_header && state.is_final_input) {
      // We've reached the end of the body of the request.
      return EDecodeStatus::kHttpOk;
    } else {
      return EDecodeStatus::kNeedMoreInput;
    }
  } else if (view.at(0) == '&') {
    view.remove_prefix(1);
    return state.SetDecodeFunction(DecodeParamName);
  } else if (view.at(0) == ' ') {
    view.remove_prefix(1);
    return state.SetDecodeFunction(MatchHttpVersion);
  } else {
    return EDecodeStatus::kHttpBadRequest;
  }
}

// Note that a parameter value may be empty, which makes detecting the end of it
// tricky if also at the end of the body of a a request.
EDecodeStatus DecodeParamValue(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeParamValue " << view.ToEscapedString();
  StringView value;
  if (!ExtractMatchingPrefix(state, view, value, IsParamValueChar)) {
    // view doesn't contain a character that can't be in a parameter value. We
    // may need more input.
    if (state.is_decoding_header || !state.is_final_input) {
      return EDecodeStatus::kNeedMoreInput;
    }
    DCHECK_EQ(state.remaining_content_length, view.size());
    value = view;
    view.remove_prefix(value.size());
  }
  DVLOG(1) << "DecodeParamValue value: " << value.ToEscapedString();
  EDecodeStatus status = EDecodeStatus::kContinueDecoding;
  if (state.current_parameter == EParameter::kClientId) {
    if (state.request.found_client_id ||
        !value.to_uint32(state.request.client_id)) {
      status = state.listener.OnExtraParameter(EParameter::kClientId, value);
      if (status <= EDecodeStatus::kHttpOk) {
        status = EDecodeStatus::kHttpBadRequest;
      }
    } else {
      state.request.found_client_id = true;
    }
  } else if (state.current_parameter == EParameter::kClientTransactionId) {
    if (state.request.found_client_transaction_id ||
        !value.to_uint32(state.request.client_transaction_id)) {
      status = state.listener.OnExtraParameter(EParameter::kClientTransactionId,
                                               value);
      if (status <= EDecodeStatus::kHttpOk) {
        status = EDecodeStatus::kHttpBadRequest;
      }
    } else {
      state.request.found_client_transaction_id = true;
    }
  } else if (state.current_parameter == EParameter::kUnknown) {
    status = state.listener.OnUnknownParameterValue(value);
  } else {
    // Recognized but no built-in support.
    status = state.listener.OnExtraParameter(state.current_parameter, value);
  }
  return state.SetDecodeFunctionAfterListenerCall(DecodeParamSeparator, status);
}

EDecodeStatus MatchParamNameValueSeparator(RequestDecoderState& state,
                                           StringView& view) {
  DVLOG(2) << "MatchParamNameValueSeparator " << view.ToEscapedString();
  if (view.empty()) {
    return EDecodeStatus::kNeedMoreInput;
  }
  if (view.at(0) != '=') {
    return EDecodeStatus::kHttpBadRequest;
  }
  view.remove_prefix(1);
  return state.SetDecodeFunction(DecodeParamValue);
}

EDecodeStatus DecodeParamName(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeParamName " << view.ToEscapedString();
  StringView name;
  if (ExtractMatchingPrefix(state, view, name, IsNameChar)) {
    state.current_parameter =
        LowerMatchTokens(name, EParameter::kUnknown, kRecognizedParameters);
    if (state.current_parameter == EParameter::kUnknown) {
      return state.SetDecodeFunctionAfterListenerCall(
          MatchParamNameValueSeparator,
          state.listener.OnUnknownParameterName(name));
    }
    return state.SetDecodeFunction(MatchParamNameValueSeparator);
  } else if (view.empty() && !state.is_decoding_header &&
             state.is_final_input) {
    // We've reached the end of the body of the request.
    return EDecodeStatus::kHttpOk;
  } else {
    return EDecodeStatus::kNeedMoreInput;
  }
}

// Decode the path, i.e. find the end of the path, which is either a '?' or
// whitespace. Note that we only support "origin-form" (as defined in RFC 7230),
// which means that the path must start with a forward slash.
EDecodeStatus DecodeAscomMethod(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeAscomMethod " << view.ToEscapedString();

  // Can we create an abstraction here (i.e. share this code with multiple
  // similar methods)?

  // Find the end of the device type, which should be a '/' or a '?'
  auto beyond = FindFirstNotOf(view, IsUnreserved);
  if (beyond == StringView::kMaxSize) {
    // The entire path (plus the next char) is not in the buffer (yet).
    return EDecodeStatus::kNeedMoreInput;
  }
  auto path_segment = view.prefix(beyond);
  const EMethod method = MatchTokensExactly(path_segment, EMethod::kUnknown,
                                            kRecognizedAscomMethods);
  if (method == EMethod::kUnknown) {
    return EDecodeStatus::kHttpNotFound;
  }
  state.request.ascom_method = method;
  const char beyond_char = view.at(beyond);
  view.remove_prefix(beyond + 1);
  if (beyond_char == '?') {
    return state.SetDecodeFunction(DecodeParamName);
  } else if (beyond_char == ' ') {
    return state.SetDecodeFunction(MatchHttpVersion);
  }
  return EDecodeStatus::kHttpBadRequest;
}

EDecodeStatus DecodeDeviceNumber(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeDeviceNumber " << view.ToEscapedString();

  // Can we create an abstraction here (i.e. share this code with multiple
  // similar methods)?

  // Find the end of the device number, which should be a '/'.
  auto beyond = FindFirstNotOf(view, IsUnreserved);
  if (beyond == StringView::kMaxSize) {
    // The entire path (plus the next char) is not in the buffer (yet).
    return EDecodeStatus::kNeedMoreInput;
  }
  if (view.at(beyond) != '/') {
    return EDecodeStatus::kHttpBadRequest;
  }
  auto path_segment = view.prefix(beyond);

  if (!path_segment.to_uint32(state.request.device_number)) {
    return EDecodeStatus::kHttpBadRequest;
  }
  view.remove_prefix(beyond + 1);
  return state.SetDecodeFunction(DecodeAscomMethod);
}

// After the path prefix, we expect the name of a supported device type.
EDecodeStatus DecodeDeviceType(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeDeviceType " << view.ToEscapedString();

  // Can we create an abstraction here (i.e. share this code with multiple
  // similar methods)?

  // Find the end of the device type, which should be a '/'.
  auto beyond = FindFirstNotOf(view, IsUnreserved);
  if (beyond == StringView::kMaxSize) {
    // The entire path (plus the next char) is not in the buffer (yet).
    return EDecodeStatus::kNeedMoreInput;
  }
  if (view.at(beyond) != '/') {
    return EDecodeStatus::kHttpBadRequest;
  }
  auto path_segment = view.prefix(beyond);
  const EDeviceType device_type = MatchTokensExactly(
      path_segment, EDeviceType::kUnknown, kRecognizedDeviceTypes);
  if (device_type == EDeviceType::kUnknown) {
    return EDecodeStatus::kHttpNotFound;
  }
  state.request.device_type = device_type;
  view.remove_prefix(beyond + 1);
  return state.SetDecodeFunction(DecodeDeviceNumber);
}

// An ASCOM Alpaca request path should always start with "/api/v1/".  We add a
// space before the path (in kAscomPathPrefix) because a space should follow the
// HTTP method name.
EDecodeStatus MatchAscomPathPrefix(RequestDecoderState& state,
                                   StringView& view) {
  DVLOG(2) << "MatchAscomPathPrefix " << view.ToEscapedString();
  if (view.match_and_consume(kAscomPathPrefix)) {
    return state.SetDecodeFunction(DecodeDeviceType);
  } else if (kAscomPathPrefix.starts_with(view)) {
    return EDecodeStatus::kNeedMoreInput;
  }
  return EDecodeStatus::kHttpBadRequest;
}

// Decode one of the few supported HTTP methods. If definitely not present,
// returns an error. We *could* allow for leading whitespace, which has been
// supported implementations in the past, perhaps to deal with multiple requests
// (or multiple responses) in a row without clear delimiters. However HTTP/1.1
// requires clear delimiters, and we're planning to support only a single
// request per TCP connection (i.e. we won't support Keep-Alive).
EDecodeStatus DecodeHttpMethod(RequestDecoderState& state, StringView& view) {
  DVLOG(2) << "DecodeHttpMethod " << view.ToEscapedString();
  StringView method_name;
  if (ExtractMatchingPrefix(state, view, method_name, IsNameChar)) {
    EHttpMethod method = MatchTokensExactly(method_name, EHttpMethod::kUnknown,
                                            kRecognizedHttpMethods);
    if (method == EHttpMethod::kUnknown) {
      return EDecodeStatus::kHttpMethodNotImplemented;
    }
    DVLOG(3) << "method: " << method;
    state.request.http_method = method;
    return state.SetDecodeFunction(MatchAscomPathPrefix);
  } else {
    return EDecodeStatus::kNeedMoreInput;
  }
}

}  // namespace

std::ostream& operator<<(std::ostream& out, DecodeFunction decode_function) {
  if (decode_function == DecodeAscomMethod) {
    return out << "DecodeAscomMethod";
  }
  if (decode_function == DecodeDeviceNumber) {
    return out << "DecodeDeviceNumber";
  }
  if (decode_function == DecodeDeviceType) {
    return out << "DecodeDeviceType";
  }
  if (decode_function == DecodeHeaderLineEnd) {
    return out << "DecodeHeaderLineEnd";
  }
  if (decode_function == DecodeHeaderLines) {
    return out << "DecodeHeaderLines";
  }
  if (decode_function == DecodeHeaderName) {
    return out << "DecodeHeaderName";
  }
  if (decode_function == DecodeHeaderValue) {
    return out << "DecodeHeaderValue";
  }
  if (decode_function == DecodeHttpMethod) {
    return out << "DecodeHttpMethod";
  }
  if (decode_function == DecodeParamName) {
    return out << "DecodeParamName";
  }
  if (decode_function == DecodeParamSeparator) {
    return out << "DecodeParamSeparator";
  }
  if (decode_function == DecodeParamValue) {
    return out << "DecodeParamValue";
  }
  if (decode_function == MatchHeaderNameValueSeparator) {
    return out << "MatchHeaderNameValueSeparator";
  }
  if (decode_function == MatchHttpVersion) {
    return out << "MatchHttpVersion";
  }
  if (decode_function == MatchParamNameValueSeparator) {
    return out << "MatchParamNameValueSeparator";
  }
  if (decode_function == MatchAscomPathPrefix) {
    return out << "MatchAscomPathPrefix";
  }
  DCHECK(false) << "Haven't implemented a case for function @"
                << std::addressof(decode_function);
  return out << "Haven't implemented a case for function @"
             << std::addressof(decode_function);
}

RequestDecoderState::RequestDecoderState(AlpacaRequest& request,
                                         RequestDecoderListener& listener)
    : decode_function(nullptr), request(request), listener(listener) {}

void RequestDecoderState::Reset() {
  DVLOG(1) << "Reset #########################################################";
  decode_function = DecodeHttpMethod;
  request.Reset();
  is_decoding_header = true;
  is_decoding_start_line = true;
  is_final_input = false;
  found_content_length = false;
}

// TODO(jamessynge): This is an annoyingly complicated function. Consider how to
// keep the functionality but make the implementation simpler. It is likely that
// doing so will increase the code size, so a trade-off of size vs.
// maintainability is likely required.
EDecodeStatus RequestDecoderState::DecodeBuffer(StringView& buffer,
                                                bool at_end_of_input) {
  DVLOG(1) << "DecodeBuffer " << buffer.ToEscapedString();
  if (decode_function == nullptr) {
    // Need to call Reset first.
    //
    // Why not call Reset automatically from the ctor? Because we assume that
    // these objects will have static storage when in the embedded system, and
    // will be used for decoding multiple requests; therefore it doesn't make
    // sense to have special behavior in the caller to omit the first call to
    // Reset.
    return EDecodeStatus::kInternalError;
  }

  EDecodeStatus status;
  if (is_decoding_header) {
    status = DecodeMessageHeader(buffer, at_end_of_input);
  } else {
    status = DecodeMessageBody(buffer, at_end_of_input);
  }

  DCHECK_NE(status, EDecodeStatus::kContinueDecoding);
  if (status >= EDecodeStatus::kHttpOk) {
    decode_function = nullptr;
  }
  DVLOG(1) << "DecodeBuffer --> " << status;
  return status;
}

// Decoding the start line, header lines, or end of header line. We don't know
// how many bytes are supposed to be in the header, so we rely on
// DecodeHeaderLines to find the end.
EDecodeStatus RequestDecoderState::DecodeMessageHeader(StringView& buffer,
                                                       bool at_end_of_input) {
  DVLOG(1) << "DecodeHead " << buffer.ToEscapedString();

  EDecodeStatus status;
  do {
#ifndef NDEBUG
    const auto buffer_size_before_decode = buffer.size();
    auto old_decode_function = decode_function;
#endif

    status = decode_function(*this, buffer);

#ifndef NDEBUG
    DCHECK_LE(buffer.size(), buffer_size_before_decode);
    auto consumed_chars = buffer_size_before_decode - buffer.size();

    DVLOG(3) << "decode_function returned " << status << ", consumed "
             << consumed_chars << " characters, decode function "
             << (old_decode_function == decode_function ? "unchanged"
                                                        : "changed");

    if (status == EDecodeStatus::kContinueDecoding) {
      DCHECK_NE(old_decode_function, decode_function)
          << "Should have changed the decode function";  // COV_NF_LINE
    }
#endif
  } while (status == EDecodeStatus::kContinueDecoding);

  if (status == EDecodeStatus::kNeedMoreInput) {
    if (!is_decoding_header) {
      // We've just finished the message header, and this is a request with a
      // body.
      return DecodeMessageBody(buffer, at_end_of_input);
    }
  }

  return status;
}

// Decode the body of a PUT request where a Content-Length header was provided
// (i.e. remaining_content_length tells us how many ASCII characters remain).
EDecodeStatus RequestDecoderState::DecodeMessageBody(StringView& buffer,
                                                     bool at_end_of_input) {
  DVLOG(1) << "DecodeMessageBody " << buffer.ToEscapedString();

  DCHECK(found_content_length);
  DCHECK_EQ(request.http_method, EHttpMethod::PUT);
  // static_assert(sizeof(int16_t) >= sizeof(StringView::size_type),
  //               "Need more storage allowed");
  // int16_t local_limit = remaining_content_length;

  if (buffer.size() > remaining_content_length) {
    // We assume that the HTTP client has not sent pipelined requests.
    DLOG(WARNING) << "There is more input than Content-Length indicated: "
                  << buffer.size() << " > " << remaining_content_length;
    return EDecodeStatus::kHttpPayloadTooLarge;
  } else if (buffer.size() == remaining_content_length) {
    at_end_of_input = true;
    is_final_input = true;
  } else {
    // buffer.size() < remaining_content_length
    if (at_end_of_input || is_final_input) {
      // The available input is not sufficient to reach Content-Length.
      //
      // TO DO Consider adding a "const char*" http_reason_phrase field to
      // Request, and setting it when we have something to say about why the
      // message failed. This could also be used in the ErrorMessage field of
      // the JSON response body.
      return EDecodeStatus::kHttpBadRequest;
    }
  }

  EDecodeStatus status;
  do {
    const auto buffer_size_before_decode = buffer.size();
#ifndef NDEBUG
    const auto old_decode_function = decode_function;
#endif

    status = decode_function(*this, buffer);
    const auto consumed_chars = buffer_size_before_decode - buffer.size();

#ifndef NDEBUG
    DVLOG(3) << "decode_function returned " << status << ", consumed "
             << consumed_chars << " characters, decode function "
             << (old_decode_function == decode_function ? "unchanged"
                                                        : "changed");
    DCHECK_LE(buffer.size(), buffer_size_before_decode);
    DCHECK_LE(consumed_chars, remaining_content_length);
    if (decode_function == old_decode_function) {
      DCHECK_NE(status, EDecodeStatus::kContinueDecoding);
    }
    if (buffer_size_before_decode == 0) {
      // We don't bother checking whether the buffer is empty at the start or
      // end of the loop because it saves a little bit of code space, in
      // exchange for another pass through the loop, thus requiring a
      // DecodeFunction to notice that there isn't enough input for it to
      // succeed.
      DCHECK_NE(status, EDecodeStatus::kContinueDecoding);
    }
#endif

    remaining_content_length -= consumed_chars;
  } while (status == EDecodeStatus::kContinueDecoding);

  DCHECK_NE(status, EDecodeStatus::kContinueDecoding);

  if (status >= EDecodeStatus::kHttpOk) {
#ifndef NDEBUG
    if (status == EDecodeStatus::kHttpOk) {
      DCHECK_EQ(remaining_content_length, 0);
      DCHECK(at_end_of_input);
    }
#endif
    return status;
  }

  DCHECK_EQ(status, EDecodeStatus::kNeedMoreInput);
  if (at_end_of_input) {
    return EDecodeStatus::kHttpBadRequest;
  }

  DCHECK_GT(remaining_content_length, 0);
  return status;
}

EDecodeStatus RequestDecoderState::SetDecodeFunction(
    const DecodeFunction func) {
  DVLOG(3) << "SetDecodeFunction(" << func << ")";
  DCHECK_NE(decode_function, nullptr);
  DCHECK_NE(decode_function, func);
  decode_function = func;
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderState::SetDecodeFunctionAfterListenerCall(
    DecodeFunction func, EDecodeStatus status) {
  DCHECK_NE(status, EDecodeStatus::kNeedMoreInput);
  if (status == EDecodeStatus::kContinueDecoding) {
    return SetDecodeFunction(func);
  } else if (static_cast<int>(status) < 100) {
    return EDecodeStatus::kInternalError;  // COV_NF_LINE
  } else {
    return status;
  }
}

}  // namespace alpaca
