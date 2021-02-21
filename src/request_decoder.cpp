// Implementation of an HTTP Request Decoder for the ASCOM Alpaca REST API.
// Author: james.synge@gmail.com

// TODO(jamessynge): Consider whether to just return 400 Bad Request for almost
// all problems with parsing, rather than including "elaborate" efforts to
// validate the input. One choice is to add a macro guard (i.e. for #ifdef
// blocks) so that we can enable the features when there is enough code space,
// but omit when there isn't.
//
// Another way to reduce the size of *this* code is to pass the buck to the
// calling code (i.e. via the listener). The only Header that we "need" to
// decode is Content-Length; we could pass all parameters and all other headers
// to the client.
//
// Author: james.synge@gmail.com

#include "request_decoder.h"

#include "config.h"
#include "platform.h"
#include "token.h"
#include "tokens.h"

// NOTE: The syntax for the query portion of a URI is not as clearly specified
// as the rest of HTTP (AFAICT), so I'm assuming that:
//
// 1) A name is composed of upper and/or lower case ASCII letters (because those
//    are used by ASCOM Alpaca);
// 2) A value is any non-control character value, excluding space.
// 3) A name is followed by '=' and then by a value.
// 4) A name may not be empty, but some values may be empty; for example,
//    ClientId must be specified in order for this decoder to translate from
//    string to int, but some other parameters have their semantics provided by
//    the calling program, so we defer validation to that calling program
// 5) The HTTP client will not send percent encoded characters; these are are
//    not detected and decoded by this decoder, and are just treated as regular
//    characters. If they're in values that must be explicitly matched or
//    decoded by this decoder, that operation is likely to fail.

namespace alpaca {
namespace {
using DecodeFunction = RequestDecoderState::DecodeFunction;
using CharMatchFunction = bool (*)(char c);

TAS_CONSTEXPR_VAR StringView kEndOfHeaderLine("\r\n");
TAS_CONSTEXPR_VAR StringView kHttp_1_1_EOL("HTTP/1.1\r\n");
TAS_CONSTEXPR_VAR StringView kAscomPathPrefix("/api/v1/");
TAS_CONSTEXPR_VAR StringView kPathSeparator("/");
TAS_CONSTEXPR_VAR StringView kPathTerminator("? ");
TAS_CONSTEXPR_VAR StringView kParamNameValueSeparator("=");
TAS_CONSTEXPR_VAR StringView kHeaderNameValueSeparator(":");

////////////////////////////////////////////////////////////////////////////////
// Helpers for decoder functions.

bool IsOptionalWhitespace(const char c) { return c == ' ' || c == '\t'; }

bool IsParamSeparator(const char c) { return c == '&'; }

// Per RFC7230, Section 3.2, Header-Fields.
bool IsFieldContent(const char c) { return isPrintable(c) || c == '\t'; }

TAS_CONSTEXPR_VAR StringView kUnreservedNonAlnum("-._~");

bool IsUnreserved(const char c) {
  return isAlphaNumeric(c) || kUnreservedNonAlnum.contains(c);
}

// Match characters in either a URI query param or a header name; actually, just
// the subset of such characters we need to match for ASCOM Alpaca. Since we
// compare matching strings against tokens to find those we're interested in,
// having this set contain extra characters for some context doesn't really
// matter.
TAS_CONSTEXPR_VAR StringView kExtraNameChars("-_");
bool IsNameChar(const char c) {
  return isAlphaNumeric(c) || kExtraNameChars.contains(c);
}

// Match characters allowed in a URL encoded parameter value, whether in the
// path or in the body of a PUT request.
TAS_CONSTEXPR_VAR StringView kExtraParamValueChars("-_=%");
bool IsParamValueChar(const char c) {
  return isAlphaNumeric(c) || kExtraParamValueChars.contains(c);
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

// Removes leading whitespace characters, returns true when the first character
// is not a whitespace.
bool SkipLeadingOptionalWhitespace(StringView& view) {
  const auto beyond = FindFirstNotOf(view, IsOptionalWhitespace);
  if (beyond == StringView::kMaxSize) {
    // They're all whitespace (or it is empty). Get rid of them. Choosing here
    // to treat this as a remove_prefix rather than a clear, so that tests see
    // that the pointer in the view is moved forward by the number of removed
    // characters.
    view.remove_prefix(view.size());
    // Since there are no characters in the view, we don't know if the next
    // input character will be a space or not, so we can't report true yet.
    return false;
  } else {
    view.remove_prefix(beyond);
    return true;
  }
}

void TrimTrailingOptionalWhitespace(StringView& view) {
  while (!view.empty() && IsOptionalWhitespace(view.back())) {
    view.remove_suffix(1);
  }
}

// Find the token in tokens that matches the view.
template <typename E, int N>
E MatchTokensExactly(const StringView& view, E unknown_id,
                     const Token<E> (&tokens)[N]) {
  for (int i = 0; i < N; ++i) {
    if (tokens[i].str == view) {
      TAS_DVLOG(3, "MatchTokensExactly matched "
                       << tokens[i].str.ToHexEscapedString() << " to "
                       << view.ToHexEscapedString() << ", returning "
                       << tokens[i].id);
      return tokens[i].id;
    }
  }
  TAS_DVLOG(3, "MatchTokensExactly unable to match "
                   << view.ToHexEscapedString() << ", returning "
                   << unknown_id);
  return unknown_id;
}

template <typename E, int N>
E LowerMatchTokens(const StringView& view, E unknown_id,
                   const Token<E> (&tokens)[N]) {
  for (int i = 0; i < N; ++i) {
    if (tokens[i].str.equals_other_lowered(view)) {
      TAS_DVLOG(3, "LowerMatchTokens matched "
                       << tokens[i].str.ToHexEscapedString() << " to "
                       << view.ToHexEscapedString() << ", returning "
                       << tokens[i].id);
      return tokens[i].id;
    }
  }
  TAS_DVLOG(3, "LowerMatchTokens unable to match " << view.ToHexEscapedString()
                                                   << ", returning "
                                                   << unknown_id);
  return unknown_id;
}

bool ExtractMatchingPrefix(StringView& view, StringView& extracted_prefix,
                           CharMatchFunction char_matcher) {
  auto beyond = FindFirstNotOf(view, char_matcher);
  TAS_DVLOG(3, "ExtractMatchingPrefix of " << view.ToHexEscapedString()
                                           << " found " << (beyond + 0)
                                           << " matching characters");
  if (beyond == StringView::kMaxSize) {
    return false;
  }
  extracted_prefix = view.prefix(beyond);
  view.remove_prefix(beyond);
  return true;
}

using NameProcessor = EDecodeStatus (*)(RequestDecoderState& state,
                                        const StringView& matched_text,
                                        StringView& remainder_view);

EDecodeStatus ExtractAndProcessName(RequestDecoderState& state,
                                    StringView& view,
                                    const StringView& valid_terminators,
                                    const NameProcessor processor,
                                    const bool consume_terminator_char,
                                    const EDecodeStatus bad_terminator_error) {
  StringView matched_text;
  if (!ExtractMatchingPrefix(view, matched_text, IsNameChar)) {
    // We didn't find a character that IsNameChar doesn't match, so we don't
    // know if we have enough input yet.
    return EDecodeStatus::kNeedMoreInput;
  }

  if (!valid_terminators.contains(view.front())) {
    // Doesn't end with something appropriate for the path to end in. Perhaps an
    // unexpected/unsupported delimiter. Reporting Not Found because the error
    // is with the path.
    return bad_terminator_error;
  } else if (consume_terminator_char) {
    // For now, we expect that:
    //    consume_terminator_char == (valid_terminators.size() ==1)
    TAS_DCHECK_EQ(valid_terminators.size(), 1);
    view.remove_prefix(1);
  }

  return processor(state, matched_text, view);
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
  // Skip leading OWS (optional whitespace: space or horizontal tab), then take
  // all of the characters matching IsFieldContent, up to the first
  // non-matching character. If we can't find a non-matching character, we need
  // more input.
  StringView value;
  if (!SkipLeadingOptionalWhitespace(view) ||
      !ExtractMatchingPrefix(view, value, IsFieldContent)) {
    return EDecodeStatus::kNeedMoreInput;
  }
  TAS_DVLOG(1, "DecodeHeaderValue raw value: " << value.ToHexEscapedString());
  // Trim OWS from the end of the header value.
  TrimTrailingOptionalWhitespace(value);
  TAS_DVLOG(1,
            "DecodeHeaderValue trimmed value: " << value.ToHexEscapedString());
  EDecodeStatus status = EDecodeStatus::kContinueDecoding;
  if (state.current_header == EHttpHeader::kHttpAccept) {
    // Not tracking whether there are multiple accept headers.
    //
    // This is not a very complete comparison (i.e. would also match
    // "xxapplication/json+xyz"), but probably sufficient for our purpose.
    if (!value.contains(StringView("application/json"))) {
      status = state.listener.OnExtraHeader(EHttpHeader::kHttpAccept, value);
      // We're taking the status from the listener, even if it is
      // kContinueDecoding, because it isn't a problem for this server if we
      // produce a JSON result that the client didn't desire to receive.
    }
  } else if (state.current_header == EHttpHeader::kHttpContentLength) {
    // Note, we "ignore" the content-length for GET; it doesn't matter if it
    // is bogus.
    uint32_t content_length = 0;
    constexpr auto kMaxContentLength =
        std::numeric_limits<decltype(state.remaining_content_length)>::max();
    const bool converted_ok = value.to_uint32(content_length);
    const bool needed = state.request.http_method == EHttpMethod::PUT;
    if (state.found_content_length || !converted_ok ||
        (content_length > kMaxContentLength && needed)) {
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
    } else if (needed) {
      // We only keep the length if we're going to use it.
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

EDecodeStatus ProcessHeaderName(RequestDecoderState& state,
                                const StringView& matched_text,
                                StringView& view) {
  state.current_header = LowerMatchTokens(matched_text, EHttpHeader::kUnknown,
                                          kRecognizedHttpHeaders);
  if (state.current_header == EHttpHeader::kUnknown) {
    return state.SetDecodeFunctionAfterListenerCall(
        DecodeHeaderValue, state.listener.OnUnknownHeaderName(matched_text));
  }
  return state.SetDecodeFunction(DecodeHeaderValue);
}

EDecodeStatus DecodeHeaderName(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kHeaderNameValueSeparator, ProcessHeaderName,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EDecodeStatus::kHttpBadRequest);
}

EDecodeStatus DecodeHeaderLines(RequestDecoderState& state, StringView& view) {
  if (view.match_and_consume(kEndOfHeaderLine)) {
    // We've reached the end of the headers.
    if (state.request.http_method == EHttpMethod::GET) {
      // The standard requires that we not examine the body of a GET request,
      // if present, so we're done.
      return EDecodeStatus::kHttpOk;
    } else if (state.request.http_method != EHttpMethod::PUT) {
      // Shouldn't get here unless support for a new method is added to
      // DecodeHttpMethod, but not to here, or else if there is a bug.
      return EDecodeStatus::kHttpInternalServerError;  // COV_NF_LINE
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
  // If there are multiple separators, treat them as one.
  const auto beyond = FindFirstNotOf(view, IsParamSeparator);
  if (beyond == StringView::kMaxSize) {
    TAS_DVLOG(3, "DecodeParamSeparator found no non-separators in "
                     << view.ToHexEscapedString());
    // All the available characters are separators, or the view is empty.
    if (!state.is_decoding_header && state.is_final_input) {
      // We've reached the end of the body of the request.
      view.remove_prefix(view.size());
      return EDecodeStatus::kHttpOk;
    }
    // We don't know if the next character will also be a separator or not, so
    // we remove all but one of the separator characters, and return here next
    // time when there is more input.
    if (view.size() > 1) {
      view.remove_prefix(view.size() - 1);
    }
    return EDecodeStatus::kNeedMoreInput;
  }

  TAS_DVLOG(3, "DecodeParamSeparator found "
                   << (beyond + 0)
                   << " separators, followed by a non-separator");
  TAS_DCHECK(!view.empty());

  // There are zero or more separators, followed by a non-separator. This means
  // that this isn't the body of a request with one of these separators as the
  // last char in the body, so we don't need to worry about that case.

  view.remove_prefix(beyond);
  if (view.front() == ' ') {
    TAS_DVLOG(3, "Found a space");
    if (state.is_decoding_header) {
      view.remove_prefix(1);
      return state.SetDecodeFunction(MatchHttpVersion);
    }
    return EDecodeStatus::kHttpBadRequest;
  } else {
    return state.SetDecodeFunction(DecodeParamName);
  }
}

// Note that a parameter value may be empty, which makes detecting the end of it
// tricky if also at the end of the body of a a request.
EDecodeStatus DecodeParamValue(RequestDecoderState& state, StringView& view) {
  StringView value;
  if (!ExtractMatchingPrefix(view, value, IsParamValueChar)) {
    // view doesn't contain a character that can't be in a parameter value. We
    // may need more input.
    if (state.is_decoding_header || !state.is_final_input) {
      return EDecodeStatus::kNeedMoreInput;
    }
    // Ah, we're decoding the body of the request, and this is last buffer of
    // input from the client, so we can treat the end of input as the separator.
    TAS_DCHECK_EQ(state.remaining_content_length, view.size());
    value = view;
    view.remove_prefix(value.size());
  }
  TAS_DVLOG(1, "DecodeParamValue value: " << value.ToHexEscapedString());
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

EDecodeStatus ProcessParamName(RequestDecoderState& state,
                               const StringView& matched_text,
                               StringView& view) {
  state.current_parameter = LowerMatchTokens(matched_text, EParameter::kUnknown,
                                             kRecognizedParameters);
  if (state.current_parameter == EParameter::kUnknown) {
    return state.SetDecodeFunctionAfterListenerCall(
        DecodeParamValue, state.listener.OnUnknownParameterName(matched_text));
  }
  return state.SetDecodeFunction(DecodeParamValue);
}

EDecodeStatus DecodeParamName(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kParamNameValueSeparator, ProcessParamName,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EDecodeStatus::kHttpBadRequest);
}

EDecodeStatus ProcessAscomMethod(RequestDecoderState& state,
                                 const StringView& matched_text,
                                 StringView& view) {
  TAS_DVLOG(3, "ProcessAscomMethod matched_text: "
                   << matched_text.ToHexEscapedString());
  // A separator/terminating character should be present after the method.
  TAS_DCHECK(!view.empty());
  const EMethod method = MatchTokensExactly(matched_text, EMethod::kUnknown,
                                            kRecognizedAscomMethods);
  if (method == EMethod::kUnknown) {
    return EDecodeStatus::kHttpNotFound;
  }
  state.request.ascom_method = method;
  DecodeFunction next_decode_function;
  if (view.front() == '?') {
    next_decode_function = DecodeParamName;
  } else if (view.front() == ' ') {
    next_decode_function = MatchHttpVersion;
  } else {
    // kPathTerminator should have exactly the characters we just matched in the
    // preceding if/else if statements, therefore this should be unreachable.
    return EDecodeStatus::kHttpInternalServerError;  // COV_NF_LINE
  }
  view.remove_prefix(1);
  return state.SetDecodeFunction(next_decode_function);
}

// Decode the path, i.e. find the end of the path, which is either a '?' or
// whitespace. Note that we only support "origin-form" (as defined in RFC 7230),
// which means that the path must start with a forward slash.
EDecodeStatus DecodeAscomMethod(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathTerminator, ProcessAscomMethod,
      /*consume_terminator_char=*/false,
      /*bad_terminator_error=*/EDecodeStatus::kHttpNotFound);
}

EDecodeStatus ProcessDeviceNumber(RequestDecoderState& state,
                                  const StringView& matched_text,
                                  StringView& view) {
  if (!matched_text.to_uint32(state.request.device_number)) {
    return EDecodeStatus::kHttpNotFound;
  }
  return state.SetDecodeFunction(DecodeAscomMethod);
}

EDecodeStatus DecodeDeviceNumber(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessDeviceNumber,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EDecodeStatus::kHttpNotFound);
}

EDecodeStatus ProcessDeviceType(RequestDecoderState& state,
                                const StringView& matched_text,
                                StringView& view) {
  const EDeviceType device_type = MatchTokensExactly(
      matched_text, EDeviceType::kUnknown, kRecognizedDeviceTypes);
  if (device_type == EDeviceType::kUnknown) {
    return EDecodeStatus::kHttpNotFound;
  }
  state.request.device_type = device_type;
  return state.SetDecodeFunction(DecodeDeviceNumber);
}

// After the path prefix, we expect the name of a supported device type.
EDecodeStatus DecodeDeviceType(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessDeviceType,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EDecodeStatus::kHttpNotFound);
}

// An ASCOM Alpaca request path should always start with "/api/v1/".
EDecodeStatus MatchAscomPathPrefix(RequestDecoderState& state,
                                   StringView& view) {
  // Could add a MatchAndConsumeLiteral for this case and the "HTTP/1.1\r\n" at
  // the end of the start line.
  if (view.match_and_consume(kAscomPathPrefix)) {
    return state.SetDecodeFunction(DecodeDeviceType);
  } else if (kAscomPathPrefix.starts_with(view)) {
    return EDecodeStatus::kNeedMoreInput;
  } else {
    // Don't know what to make of the 'path', but it doesn't start with
    // "/api/v1/".
    return EDecodeStatus::kHttpNotFound;
  }
}

// Process the word at the start of the request, which should be the HTTP method
// name.
EDecodeStatus ProcessHttpMethod(RequestDecoderState& state,
                                const StringView& matched_text,
                                StringView& view) {
  EHttpMethod method = MatchTokensExactly(matched_text, EHttpMethod::kUnknown,
                                          kRecognizedHttpMethods);
  if (method == EHttpMethod::kUnknown) {
    return EDecodeStatus::kHttpMethodNotImplemented;
  }
  TAS_DVLOG(3, "method: " << method);
  state.request.http_method = method;
  return state.SetDecodeFunction(MatchAscomPathPrefix);
}

// Decode one of the few supported HTTP methods. If definitely not present,
// returns an error. We *could* allow for leading whitespace, which has been
// supported implementations in the past, perhaps to deal with multiple requests
// (or multiple responses) in a row without clear delimiters. However HTTP/1.1
// requires clear delimiters, and we're planning to support only a single
// request per TCP connection (i.e. we won't support Keep-Alive).
EDecodeStatus DecodeHttpMethod(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, " ", ProcessHttpMethod,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EDecodeStatus::kHttpBadRequest);
}

}  // namespace

#if TAS_ENABLE_DEBUGGING
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
  if (decode_function == MatchHttpVersion) {
    return out << "MatchHttpVersion";
  }
  if (decode_function == MatchAscomPathPrefix) {
    return out << "MatchAscomPathPrefix";
  }
  // COV_NF_START
  TAS_DCHECK(false, "Haven't implemented a case for function @"
                        << std::addressof(decode_function));
  return out << "Haven't implemented a case for function @"
             << std::addressof(decode_function);
  // COV_NF_END
}
#endif

RequestDecoderState::RequestDecoderState(AlpacaRequest& request,
                                         RequestDecoderListener& listener)
    : decode_function(nullptr), request(request), listener(listener) {}

void RequestDecoderState::Reset() {
  TAS_DVLOG(1,
            "Reset ##########################################################");
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
                                                const bool buffer_is_full,
                                                const bool at_end_of_input) {
  TAS_DVLOG(1, "DecodeBuffer " << buffer.ToHexEscapedString());
  if (decode_function == nullptr) {
    // Need to call Reset first.
    //
    // Why not call Reset automatically from the ctor? Because we assume that
    // these objects will have static storage when in the embedded system, and
    // will be used for decoding multiple requests; therefore it doesn't make
    // sense to have special behavior in the caller to omit the first call to
    // Reset.
    return EDecodeStatus::kHttpInternalServerError;
  }

  const auto start_size = buffer.size();
  EDecodeStatus status;
  if (is_decoding_header) {
    status = DecodeMessageHeader(buffer, at_end_of_input);
  } else {
    status = DecodeMessageBody(buffer, at_end_of_input);
  }
  TAS_DCHECK_NE(status, EDecodeStatus::kContinueDecoding);

  if (buffer_is_full && status == EDecodeStatus::kNeedMoreInput &&
      start_size == buffer.size()) {
    TAS_DVLOG(1,
              "Need more input, but buffer is already full (has no room for "
              "additional input).");
    status = EDecodeStatus::kHttpRequestHeaderFieldsTooLarge;
  }
  if (status >= EDecodeStatus::kHttpOk) {
    decode_function = nullptr;
  }
  TAS_DVLOG(1, "DecodeBuffer --> " << status);
  return status;
}

// Decoding the start line, header lines, or end of header line. We don't know
// how many bytes are supposed to be in the header, so we rely on
// DecodeHeaderLines to find the end.
EDecodeStatus RequestDecoderState::DecodeMessageHeader(
    StringView& buffer, const bool at_end_of_input) {
  TAS_DVLOG(1, "DecodeMessageHeader " << buffer.ToHexEscapedString());

  EDecodeStatus status;
  do {
#if TAS_ENABLE_DEBUGGING
    const auto buffer_size_before_decode = buffer.size();
    auto old_decode_function = decode_function;
    TAS_DVLOG(2, decode_function << "(" << buffer.ToHexEscapedString() << " ("
                                 << static_cast<size_t>(buffer.size())
                                 << " chars))");
#endif

    status = decode_function(*this, buffer);

#if TAS_ENABLE_DEBUGGING
    TAS_DCHECK_LE(buffer.size(), buffer_size_before_decode);
    auto consumed_chars = buffer_size_before_decode - buffer.size();

    TAS_DVLOG(3, "decode_function returned "
                     << status << ", consumed " << consumed_chars
                     << " characters, decode function "
                     << (old_decode_function == decode_function ? "unchanged"
                                                                : "changed"));

    if (status == EDecodeStatus::kContinueDecoding) {
      // This is a check on the currently expected behavior; none of the current
      // decode functions represents a loop all by itself, which isn't handled
      // inside the decode function; i.e. none of them extract some data, then
      // return kContinueDecoding without also calling SetDecodeFunction to
      // specify the next (different) function to handle the decoding.
      TAS_DCHECK_NE(old_decode_function, decode_function,
                    "Should have changed the decode function");  // COV_NF_LINE
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
  TAS_DVLOG(1, "DecodeMessageBody " << buffer.ToHexEscapedString());
  TAS_DCHECK(found_content_length);
  TAS_DCHECK_EQ(request.http_method, EHttpMethod::PUT);

  if (buffer.size() > remaining_content_length) {
    // We assume that the HTTP client has not sent pipelined requests.
    TAS_DLOG(WARNING, "There is more input than Content-Length indicated: "
                          << buffer.size() << " > "
                          << remaining_content_length);
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
#if TAS_ENABLE_DEBUGGING
    const auto old_decode_function = decode_function;
    TAS_DVLOG(2, decode_function << "(" << buffer.ToHexEscapedString() << " ("
                                 << (buffer.size() + 0) << " chars))");
#endif

    status = decode_function(*this, buffer);
    const auto consumed_chars = buffer_size_before_decode - buffer.size();

#if TAS_ENABLE_DEBUGGING
    TAS_DVLOG(3, "decode_function returned "
                     << status << ", consumed " << consumed_chars
                     << " characters, decode function "
                     << (old_decode_function == decode_function ? "unchanged"
                                                                : "changed"));
    TAS_DCHECK_LE(buffer.size(), buffer_size_before_decode);
    TAS_DCHECK_LE(consumed_chars, remaining_content_length);
    if (decode_function == old_decode_function) {
      TAS_DCHECK_NE(status, EDecodeStatus::kContinueDecoding);
    }
    if (buffer_size_before_decode == 0) {
      // We don't bother checking whether the buffer is empty at the start or
      // end of the loop because it saves a little bit of code space, in
      // exchange for another pass through the loop, thus requiring a
      // DecodeFunction to notice that there isn't enough input for it to
      // succeed.
      TAS_DCHECK_NE(status, EDecodeStatus::kContinueDecoding);
    }
#endif

    remaining_content_length -= consumed_chars;
  } while (status == EDecodeStatus::kContinueDecoding);

  TAS_DCHECK_NE(status, EDecodeStatus::kContinueDecoding);

  if (status >= EDecodeStatus::kHttpOk) {
#if TAS_ENABLE_DEBUGGING
    if (status == EDecodeStatus::kHttpOk) {
      TAS_DCHECK_EQ(remaining_content_length, 0);
      TAS_DCHECK(at_end_of_input);
    }
#endif
    return status;
  }

  TAS_DCHECK_EQ(status, EDecodeStatus::kNeedMoreInput);
  if (at_end_of_input) {
    return EDecodeStatus::kHttpBadRequest;
  }

  TAS_DCHECK_GT(remaining_content_length, 0);
  return status;
}

EDecodeStatus RequestDecoderState::SetDecodeFunction(
    const DecodeFunction func) {
  TAS_DVLOG(3, "SetDecodeFunction(" << func << ")");
  TAS_DCHECK_NE(decode_function, nullptr);
  TAS_DCHECK_NE(decode_function, func);
  decode_function = func;
  return EDecodeStatus::kContinueDecoding;
}

EDecodeStatus RequestDecoderState::SetDecodeFunctionAfterListenerCall(
    DecodeFunction func, EDecodeStatus status) {
  TAS_DCHECK_NE(status, EDecodeStatus::kNeedMoreInput);
  if (status == EDecodeStatus::kContinueDecoding) {
    return SetDecodeFunction(func);
  } else if (static_cast<int>(status) < 100) {
    return EDecodeStatus::kHttpInternalServerError;  // COV_NF_LINE
  } else {
    return status;
  }
}

}  // namespace alpaca
