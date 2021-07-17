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

#ifndef REQUEST_DECODER_ENABLE_TAS_VLOG
#define TAS_DISABLE_VLOG
#endif

#include "request_decoder.h"

#include "config.h"
#include "constants.h"
#include "literals.h"
#include "match_literals.h"
#include "utils/hex_escape.h"
#include "utils/logging.h"
#include "utils/string_compare.h"

// NOTE: The syntax for the query portion of a URI is not as clearly specified
// as the rest of HTTP (AFAICT), so I'm assuming that:
//
// 1) A name is composed of upper and/or lower case ASCII letters (because those
//    are used by ASCOM Alpaca);
// 2) A value is any non-control character value, excluding space.
// 3) A name is followed by '=' and then by a value.
// 4) A name may not be empty, but some values may be empty; for example,
//    ClientID must be specified in order for this decoder to translate from
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

// TODO(jamessynge): Decide whether to move more of these strings into literals
// or inline them somehow.
TAS_CONSTEXPR_VAR StringView kHttpMethodTerminators(" ");
TAS_CONSTEXPR_VAR StringView kEndOfHeaderLine("\r\n");
TAS_CONSTEXPR_VAR StringView kPathSeparator("/");
TAS_CONSTEXPR_VAR StringView kPathTerminators("? ");
TAS_CONSTEXPR_VAR StringView kParamNameValueSeparator("=");
TAS_CONSTEXPR_VAR StringView kHeaderNameValueSeparator(":");

////////////////////////////////////////////////////////////////////////////////
// Helpers for decoder functions.

bool HttpMethodIsRead(EHttpMethod method) {
  return method == EHttpMethod::GET || method == EHttpMethod::HEAD;
}

bool HttpMethodHasBody(EHttpMethod method) {
  return method == EHttpMethod::PUT;
}

bool IsOptionalWhitespace(const char c) { return c == ' ' || c == '\t'; }

bool IsParamSeparator(const char c) { return c == '&'; }

// Per RFC7230, Section 3.2, Header-Fields.
bool IsFieldContent(const char c) { return isPrintable(c) || c == '\t'; }

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
TAS_CONSTEXPR_VAR StringView kExtraParamValueChars("-+_=%.");
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

bool ExtractMatchingPrefix(StringView& view, StringView& extracted_prefix,
                           CharMatchFunction char_matcher) {
  auto beyond = FindFirstNotOf(view, char_matcher);
  TAS_VLOG(3) << TAS_FLASHSTR("ExtractMatchingPrefix of ") << HexEscaped(view)
              << TAS_FLASHSTR(" found ") << (beyond + 0)
              << TAS_FLASHSTR(" matching characters");
  if (beyond == StringView::kMaxSize) {
    return false;
  }
  extracted_prefix = view.prefix(beyond);
  view.remove_prefix(beyond);
  return true;
}

using NameProcessor = EHttpStatusCode (*)(RequestDecoderState& state,
                                          const StringView& matched_text,
                                          StringView& remainder_view);

EHttpStatusCode ExtractAndProcessName(
    RequestDecoderState& state, StringView& view,
    const StringView& valid_terminating_chars, const NameProcessor processor,
    const bool consume_terminator_char,
    const EHttpStatusCode bad_terminator_error) {
  TAS_DCHECK(!valid_terminating_chars.empty());
  TAS_DCHECK_GT(bad_terminator_error, EHttpStatusCode::kHttpOk);
  StringView matched_text;
  if (!ExtractMatchingPrefix(view, matched_text, IsNameChar)) {
    // We didn't find a character that IsNameChar doesn't match, so we don't
    // know if we have enough input yet.
    return EHttpStatusCode::kNeedMoreInput;
  }
  TAS_DCHECK(!view.empty());

  if (!valid_terminating_chars.contains(view.front())) {
    // Doesn't end with something appropriate for the path to end in. Perhaps an
    // unexpected/unsupported delimiter. Reporting Not Found because the error
    // is with the path.
    return bad_terminator_error;
  } else if (consume_terminator_char) {
    // For now, we expect that:
    //    consume_terminator_char == (valid_terminating_chars.size() ==1)
    TAS_DCHECK_EQ(valid_terminating_chars.size(), 1);
    view.remove_prefix(1);
  }

  return processor(state, matched_text, view);
}

EHttpStatusCode ExtractAndProcessName(RequestDecoderState& state,
                                      StringView& view,
                                      const NameProcessor processor) {
  StringView matched_text;
  if (!ExtractMatchingPrefix(view, matched_text, IsNameChar)) {
    // We didn't find a character that IsNameChar doesn't match, so we don't
    // know if we have enough input yet.
    return EHttpStatusCode::kNeedMoreInput;
  }
  TAS_DCHECK(!view.empty());
  return processor(state, matched_text, view);
}

////////////////////////////////////////////////////////////////////////////////
// Decoder functions for different phases of decoding. Generally in reverse
// order to avoid forward declarations.

// Necessary forward declarations (whereever we have a cycle in the grammar).
EHttpStatusCode DecodeParamName(RequestDecoderState& state, StringView& view);
EHttpStatusCode DecodeHeaderLines(RequestDecoderState& state, StringView& view);

EHttpStatusCode DecodeHeaderLineEnd(RequestDecoderState& state,
                                    StringView& view) {
  // We expect "\r\n" at the end of a header line.
  if (view.match_and_consume(kEndOfHeaderLine)) {
    return state.SetDecodeFunction(DecodeHeaderLines);
  } else if (kEndOfHeaderLine.starts_with(view)) {
    // Need more input.
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    // The header line doesn't end where or as expected; perhaps the EOL
    // terminator isn't correct (e.g. a "\n" instead of a  "\r\n").
    return EHttpStatusCode::kHttpBadRequest;
  }
}

EHttpStatusCode DecodeHeaderValue(RequestDecoderState& state,
                                  StringView& view) {
  // Skip leading OWS (optional whitespace: space or horizontal tab), then take
  // all of the characters matching IsFieldContent, up to the first
  // non-matching character. If we can't find a non-matching character, we need
  // more input.
  StringView value;
  if (!SkipLeadingOptionalWhitespace(view) ||
      !ExtractMatchingPrefix(view, value, IsFieldContent)) {
    return EHttpStatusCode::kNeedMoreInput;
  }
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeHeaderValue raw value: ")
              << HexEscaped(value);
  // Trim OWS from the end of the header value.
  TrimTrailingOptionalWhitespace(value);
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeHeaderValue trimmed value: ")
              << HexEscaped(value);
  EHttpStatusCode status = EHttpStatusCode::kContinueDecoding;
  if (state.current_header == EHttpHeader::kContentLength) {
    uint32_t content_length = 0;
    const bool converted_ok = value.to_uint32(content_length);
    if (state.found_content_length || !converted_ok ||
        content_length > RequestDecoderState::kMaxPayloadSize) {
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
      if (state.listener) {
        status = state.listener->OnExtraHeader(EHttpHeader::kHttpContentLength,
                                               value);
      }
      if (status <= EHttpStatusCode::kHttpOk) {
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
        if (content_length > 0) {
          // It's out of range for our decoder.
          status = EHttpStatusCode::kHttpPayloadTooLarge;
        } else {
          status = EHttpStatusCode::kHttpBadRequest;
        }
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
      }
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
    } else if (content_length > 0 &&
               !HttpMethodHasBody(state.request.http_method)) {
      // We could choose to skip over the payload of the request, but we don't
      // know if the client intended the payload to have meaning. Thus, we
      // reject a request with an unexpected payload. For more info, see:
      // https://tools.ietf.org/html/rfc7231#section-4.3.1
      status = EHttpStatusCode::kHttpBadRequest;
    } else {
      state.remaining_content_length = content_length;
      state.found_content_length = true;
    }
  } else if (state.current_header == EHttpHeader::kContentType) {
    // Note that the syntax for the Content-Type header is more complex than
    // allowed for here; after the media-type there may be a semi-colon and some
    // additional details (charset and boundary).
    if (state.request.http_method == EHttpMethod::PUT &&
        value != Literals::MimeTypeWwwFormUrlEncoded()) {
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
      if (state.listener) {
        status =
            state.listener->OnExtraHeader(EHttpHeader::kHttpContentType, value);
      } else {
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
        status = EHttpStatusCode::kHttpUnsupportedMediaType;
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
      }
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
    }
  } else if (state.current_header == EHttpHeader::kConnection) {
    if (CaseEqual(Literals::close(), value)) {
      state.request.do_close = true;
    }
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
  } else if (state.current_header == EHttpHeader::kUnknown) {
    if (state.listener) {
      status = state.listener->OnUnknownHeaderValue(value);
    }
  } else {
    // Recognized but no built-in support.
    if (state.listener) {
      status = state.listener->OnExtraHeader(state.current_header, value);
    }
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  }
  return state.SetDecodeFunctionAfterListenerCall(DecodeHeaderLineEnd, status);
}

// We've determined that we don't need to examine the value of this header, so
// we just skip forward to the end of the line. This allows us to skip past
// large headers (e.g. User-Agent) whose value we don't care about.
EHttpStatusCode SkipHeaderValue(RequestDecoderState& state, StringView& view) {
  while (!view.empty()) {
    if (view.starts_with('\r')) {
      return state.SetDecodeFunction(DecodeHeaderLineEnd);
    }
    view.remove_prefix(1);
  }
  return EHttpStatusCode::kNeedMoreInput;
}

EHttpStatusCode ProcessHeaderName(RequestDecoderState& state,
                                  const StringView& matched_text,
                                  StringView& view) {
  state.current_header = EHttpHeader::kUnknown;
  if (!MatchHttpHeader(matched_text, state.current_header)) {
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
    EHttpStatusCode status = EHttpStatusCode::kContinueDecoding;
    if (state.listener) {
      status = state.listener->OnUnknownHeaderName(matched_text);
    }
    // TODO(jamessynge): Define a better API for handling LONG header values,
    // i.e. those which can not be buffered on an Arduino, such as Accept.
    return state.SetDecodeFunctionAfterListenerCall(DecodeHeaderValue, status);
#else
    return state.SetDecodeFunction(SkipHeaderValue);
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  }
  return state.SetDecodeFunction(DecodeHeaderValue);
}

EHttpStatusCode DecodeHeaderName(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kHeaderNameValueSeparator, ProcessHeaderName,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode DecodeHeaderLines(RequestDecoderState& state,
                                  StringView& view) {
  if (view.match_and_consume(kEndOfHeaderLine)) {
    // We've reached the end of the headers.
    if (state.request.http_method == EHttpMethod::GET) {
      // The standard requires that we not examine the body of a GET request,
      // if present, so we're done.
      return EHttpStatusCode::kHttpOk;
    } else if (state.request.http_method != EHttpMethod::PUT) {
      // Shouldn't get here unless support for a new method is added to
      // DecodeHttpMethod, but not to here, or else if there is a bug.
      return EHttpStatusCode::kHttpInternalServerError;  // COV_NF_LINE
    } else if (!state.found_content_length) {
      // We need to know the length in order to decode the body.
      return EHttpStatusCode::kHttpLengthRequired;
    } else if (state.remaining_content_length == 0) {
      // Very odd, but it is possible that all of the parameters are in the
      // query parameters in the start line of the request. For example, the
      // "refresh" method of the "observingconditions" device type requires no
      // parameters.
      return EHttpStatusCode::kHttpOk;
    } else {
      // There is a body of known length to be decoded.
      state.is_decoding_header = false;
      state.decode_function = DecodeParamName;
      return EHttpStatusCode::kNeedMoreInput;
    }
  } else if (kEndOfHeaderLine.starts_with(view)) {
    // view might be empty, or it might be the start of a kEndOfHeaderLine.
    // To decide what to do next, we need more input.
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    return state.SetDecodeFunction(DecodeHeaderName);
  }
}

// An HTTP/1.1 Request Start Line should always end with "HTTP/1.1\r\n".
// (We're not supporting HTTP/1.0 or earlier.)
EHttpStatusCode MatchHttpVersion(RequestDecoderState& state, StringView& view) {
  const Literal expected = Literals::HttpVersionEndOfLine();
  if (StartsWith(view, expected)) {
    view.remove_prefix(expected.size());
    state.is_decoding_start_line = false;
    return state.SetDecodeFunction(DecodeHeaderLines);
  } else if (view.size() < expected.size()) {
    return EHttpStatusCode::kNeedMoreInput;
  } else {
    return EHttpStatusCode::kHttpVersionNotSupported;
  }
}

EHttpStatusCode DecodeParamSeparator(RequestDecoderState& state,
                                     StringView& view) {
  // If there are multiple separators, treat them as one.
  const auto beyond = FindFirstNotOf(view, IsParamSeparator);
  if (beyond == StringView::kMaxSize) {
    TAS_VLOG(3) << TAS_FLASHSTR(
                       "DecodeParamSeparator found no non-separators in ")
                << HexEscaped(view);
    // All the available characters are separators, or the view is empty.
    if (!state.is_decoding_header && state.is_final_input) {
      // We've reached the end of the body of the request.
      view.remove_prefix(view.size());
      return EHttpStatusCode::kHttpOk;
    }
    // We don't know if the next character will also be a separator or not, so
    // we remove all but one of the separator characters, and return here next
    // time when there is more input.
    if (view.size() > 1) {
      view.remove_prefix(view.size() - 1);
    }
    return EHttpStatusCode::kNeedMoreInput;
  }

  // There are zero or more separators, followed by a non-separator. This means
  // that this isn't the body of a request with one of these separators as the
  // last char in the body, so we don't need to worry about that case.
  TAS_VLOG(3) << TAS_FLASHSTR("DecodeParamSeparator found ") << (beyond + 0)
              << TAS_FLASHSTR(" separators, followed by a non-separator");
  TAS_DCHECK(!view.empty());

  view.remove_prefix(beyond);
  if (view.front() == ' ') {
    if (state.is_decoding_header) {
      view.remove_prefix(1);
      return state.SetDecodeFunction(MatchHttpVersion);
    }
    TAS_VLOG(3) << TAS_FLASHSTR("Found an unexpected space in body");
    return EHttpStatusCode::kHttpBadRequest;
  } else {
    return state.SetDecodeFunction(DecodeParamName);
  }
}

EHttpStatusCode ReportExtraParameter(RequestDecoderState& state,
                                     StringView value) {
  EHttpStatusCode status = EHttpStatusCode::kHttpBadRequest;
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
  if (state.listener) {
    status = state.listener->OnExtraParameter(state.current_parameter, value);
    if (status <= EHttpStatusCode::kHttpOk) {
      status = EHttpStatusCode::kHttpBadRequest;
    }
  }
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  return status;
}

// Note that a parameter value may be empty, which makes detecting the end of it
// tricky if also at the end of the body of a request.
EHttpStatusCode DecodeParamValue(RequestDecoderState& state, StringView& view) {
  StringView value;
  if (!ExtractMatchingPrefix(view, value, IsParamValueChar)) {
    // view doesn't contain a character that can't be in a parameter value. We
    // may need more input.
    if (state.is_decoding_header || !state.is_final_input) {
      return EHttpStatusCode::kNeedMoreInput;
    }
    // Ah, we're decoding the body of the request, and this is last buffer of
    // input from the client, so we can treat the end of input as the separator.
    TAS_DCHECK_EQ(state.remaining_content_length, view.size());
    value = view;
    view.remove_prefix(value.size());
  }
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeParamValue param: ")
              << state.current_parameter << TAS_FLASHSTR(", value: ")
              << HexEscaped(value);
  EHttpStatusCode status = EHttpStatusCode::kContinueDecoding;
  if (state.current_parameter == EParameter::kClientID) {
    uint32_t id;
    bool converted_ok = value.to_uint32(id);
    if (state.request.have_client_id || !converted_ok) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.set_client_id(id);
    }
  } else if (state.current_parameter == EParameter::kClientTransactionID) {
    uint32_t id;
    bool converted_ok = value.to_uint32(id);
    if (state.request.have_client_transaction_id || !converted_ok) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.set_client_transaction_id(id);
    }
  } else if (state.current_parameter == EParameter::kId) {
    uint32_t id;
    bool converted_ok = value.to_uint32(id);
    if (state.request.have_id || !converted_ok) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.set_id(id);
    }
  } else if (state.current_parameter == EParameter::kBrightness) {
    int32_t brightness;
    bool converted_ok = value.to_int32(brightness);
    if (state.request.have_brightness || !converted_ok) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.set_brightness(brightness);
    }
  } else if (state.current_parameter == EParameter::kValue) {
    double d;
    bool converted_ok = value.to_double(d);
    if (state.request.have_value || !converted_ok) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.set_value(d);
    }
  } else if (state.current_parameter == EParameter::kAveragePeriod) {
    double d;
    bool converted_ok = value.to_double(d);
    if (state.request.have_average_period || !converted_ok) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.set_average_period(d);
    }
  } else if (state.current_parameter == EParameter::kConnected) {
    if (CaseEqual(value, Literals::False()) && !state.request.have_connected) {
      state.request.set_connected(false);
    } else if (CaseEqual(value, Literals::True()) &&
               !state.request.have_connected) {
      state.request.set_connected(true);
    } else {
      status = ReportExtraParameter(state, value);
    }
  } else if (state.current_parameter == EParameter::kState) {
    if (CaseEqual(value, Literals::False()) && !state.request.have_state) {
      state.request.set_state(false);
    } else if (CaseEqual(value, Literals::True()) &&
               !state.request.have_state) {
      state.request.set_state(true);
    } else {
      status = ReportExtraParameter(state, value);
    }
  } else if (state.current_parameter == EParameter::kSensorName) {
    ESensorName matched;
    if (state.request.sensor_name != ESensorName::kUnknown ||
        !MatchSensorName(value, matched)) {
      status = ReportExtraParameter(state, value);
    } else {
      state.request.sensor_name = matched;
    }
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
  } else if (state.current_parameter == EParameter::kUnknown) {
    if (state.listener) {
      status = state.listener->OnUnknownParameterValue(value);
    }
  } else {
    // Recognized but no built-in support.
    if (state.listener) {
      status = state.listener->OnExtraParameter(state.current_parameter, value);
    }
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  }
  return state.SetDecodeFunctionAfterListenerCall(DecodeParamSeparator, status);
}

EHttpStatusCode ProcessParamName(RequestDecoderState& state,
                                 const StringView& matched_text,
                                 StringView& view) {
  state.current_parameter = EParameter::kUnknown;
  if (MatchParameter(matched_text, state.current_parameter)) {
    return state.SetDecodeFunction(DecodeParamValue);
  }
  // Unrecognized parameter name.
  EHttpStatusCode status = EHttpStatusCode::kContinueDecoding;
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
  if (state.listener) {
    status = state.listener->OnUnknownParameterName(matched_text);
  }
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
  return state.SetDecodeFunctionAfterListenerCall(DecodeParamValue, status);
}

EHttpStatusCode DecodeParamName(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kParamNameValueSeparator, ProcessParamName,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

// We've read what should be the final segment of the path, and expect either a
// '?' marking the beginning of a query (i.e. parameter names and values), or
// the ' ' (space) that appears before the HTTP version number.
EHttpStatusCode DecodeEndOfPath(RequestDecoderState& state, StringView& view) {
  // A separator/terminating character should be present, else we would not have
  // been able to determine that the previous segment was done.
  TAS_DCHECK(!view.empty());
  DecodeFunction next_decode_function;
  if (view.match_and_consume('?')) {
    next_decode_function = DecodeParamName;
  } else if (view.match_and_consume(' ')) {
    next_decode_function = MatchHttpVersion;
  } else {
    // We expected the path to end, but maybe the client send more path
    // segments?
    return EHttpStatusCode::kHttpBadRequest;
  }
  return state.SetDecodeFunction(next_decode_function);
}

EHttpStatusCode ProcessDeviceMethod(RequestDecoderState& state,
                                    const StringView& matched_text,
                                    StringView& view) {
  TAS_VLOG(3) << TAS_FLASHSTR("ProcessDeviceMethod matched_text: ")
              << HexEscaped(matched_text);

  // A separator/terminating character should be present after the method.
  TAS_CHECK(!view.empty());

  EDeviceMethod method;
  if (MatchDeviceMethod(state.request.api_group, state.request.device_type,
                        matched_text, method)) {
    TAS_DCHECK(method == EDeviceMethod::kSetup ||
               state.request.api == EAlpacaApi::kDeviceApi)
        << TAS_FLASHSTR("Wrong combo: method=") << method
        << TAS_FLASHSTR(", api=") << state.request.api;
    TAS_DCHECK(method != EDeviceMethod::kSetup ||
               state.request.api == EAlpacaApi::kDeviceSetup)
        << TAS_FLASHSTR("Wrong combo: method=") << method
        << TAS_FLASHSTR(", api=") << state.request.api;
    state.request.device_method = method;
    return state.SetDecodeFunction(DecodeEndOfPath);
  }
  return EHttpStatusCode::kHttpBadRequest;
}

EHttpStatusCode DecodeDeviceMethod(RequestDecoderState& state,
                                   StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathTerminators, ProcessDeviceMethod,
      /*consume_terminator_char=*/false,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode ProcessDeviceNumber(RequestDecoderState& state,
                                    const StringView& matched_text,
                                    StringView& view) {
  if (!matched_text.to_uint32(state.request.device_number)) {
    return EHttpStatusCode::kHttpBadRequest;
  }
  return state.SetDecodeFunction(DecodeDeviceMethod);
}

EHttpStatusCode DecodeDeviceNumber(RequestDecoderState& state,
                                   StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessDeviceNumber,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode ProcessDeviceType(RequestDecoderState& state,
                                  const StringView& matched_text,
                                  StringView& view) {
  EDeviceType device_type;
  if (MatchDeviceType(matched_text, device_type)) {
    TAS_VLOG(3) << TAS_FLASHSTR("device_type: ") << device_type;
    state.request.device_type = device_type;
    return state.SetDecodeFunction(DecodeDeviceNumber);
  }
  return EHttpStatusCode::kHttpBadRequest;
}

// After the path prefix, we expect the name of a supported device type.
EHttpStatusCode DecodeDeviceType(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessDeviceType,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

// Process the word that starts the path (i.e. right after the leading /).
EHttpStatusCode ProcessApiVersion(RequestDecoderState& state,
                                  const StringView& matched_text,
                                  StringView& view) {
  if (matched_text == Literals::v1()) {
    return state.SetDecodeFunction(DecodeDeviceType);
  } else {
    return EHttpStatusCode::kHttpBadRequest;
  }
}

EHttpStatusCode DecodeApiVersion(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kPathSeparator, ProcessApiVersion,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

EHttpStatusCode ProcessManagementMethod(RequestDecoderState& state,
                                        const StringView& matched_text,
                                        StringView& view) {
  TAS_DCHECK(!view.empty());
  EManagementMethod method;
  if (MatchManagementMethod(matched_text, method)) {
    TAS_VLOG(3) << TAS_FLASHSTR("method: ") << method;
    if (method == EManagementMethod::kDescription) {
      state.request.api = EAlpacaApi::kManagementDescription;
    } else if (method == EManagementMethod::kConfiguredDevices) {
      state.request.api = EAlpacaApi::kManagementConfiguredDevices;
    } else {
      // COV_NF_START
      TAS_DCHECK(false) << TAS_FLASHSTR("method (") << method
                        << TAS_FLASHSTR(") unexpected");
      return EHttpStatusCode::kHttpInternalServerError;
      // COV_NF_END
    }
    return state.SetDecodeFunction(DecodeEndOfPath);
  }
  return EHttpStatusCode::kHttpBadRequest;
}

EHttpStatusCode DecodeManagementMethod(RequestDecoderState& state,
                                       StringView& view) {
  return ExtractAndProcessName(state, view, ProcessManagementMethod);
}

// What kind of management operation is this?
EHttpStatusCode ProcessManagementType(RequestDecoderState& state,
                                      const StringView& matched_text,
                                      StringView& view) {
  TAS_DCHECK(!view.empty());
  if (matched_text == Literals::v1()) {
    if (view.match_and_consume('/')) {
      return state.SetDecodeFunction(DecodeManagementMethod);
    } else {
      return EHttpStatusCode::kHttpBadRequest;
    }
  } else if (matched_text == Literals::apiversions()) {
    state.request.api = EAlpacaApi::kManagementApiVersions;
    return state.SetDecodeFunction(DecodeEndOfPath);
  } else {
    return EHttpStatusCode::kHttpBadRequest;
  }
}

// The path starts "/management/". What's next?
EHttpStatusCode DecodeManagementType(RequestDecoderState& state,
                                     StringView& view) {
  return ExtractAndProcessName(state, view, ProcessManagementType);
}

// Process the word that starts the path, right after the leading '/'.
EHttpStatusCode ProcessApiGroup(RequestDecoderState& state,
                                const StringView& matched_text,
                                StringView& view) {
  TAS_DCHECK(!view.empty());
  EApiGroup group;
  if (!MatchApiGroup(matched_text, group)) {
    return EHttpStatusCode::kHttpBadRequest;
  }
  state.request.api_group = group;
  if (view.match_and_consume('/')) {
    // The path continues.
    // NOTE: If adding support for more paths (e.g. a PUT or POST request to
    // handle updating parameters in EEPROM), we'll need to adjust this code.
    if (!HttpMethodIsRead(state.request.http_method) &&
        group != EApiGroup::kDevice) {
      return EHttpStatusCode::kHttpMethodNotAllowed;
    }
    if (group == EApiGroup::kManagement) {
      return state.SetDecodeFunction(DecodeManagementType);
    } else if (group == EApiGroup::kSetup) {
      state.request.api = EAlpacaApi::kDeviceSetup;
    } else {
      TAS_DCHECK_EQ(group, EApiGroup::kDevice);
      state.request.api = EAlpacaApi::kDeviceApi;
    }
    TAS_DCHECK(group == EApiGroup::kDevice || group == EApiGroup::kSetup)
        << TAS_FLASHSTR("group: ") << group;
    return state.SetDecodeFunction(DecodeApiVersion);
  }
  if (group != EApiGroup::kSetup) {
    return EHttpStatusCode::kHttpBadRequest;
  }
  state.request.api = EAlpacaApi::kServerSetup;
  if (!HttpMethodIsRead(state.request.http_method)) {
    return EHttpStatusCode::kHttpMethodNotAllowed;
  }
  // We appear to have reached the end of the path. Handle what comes
  // next.
  return state.SetDecodeFunction(DecodeEndOfPath);
}

// After the '/' at the start of a path, we expect the name of an API group.
EHttpStatusCode DecodeApiGroup(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(state, view, ProcessApiGroup);
}

// View should start with '/', once we have at least a character of input.
EHttpStatusCode MatchStartOfPath(RequestDecoderState& state, StringView& view) {
  if (view.empty()) {
    return EHttpStatusCode::kNeedMoreInput;
  } else if (view.match_and_consume('/')) {
    return state.SetDecodeFunction(DecodeApiGroup);
  } else {
    // Don't know what to make of the 'path': it doesn't start with "/".
    return EHttpStatusCode::kHttpBadRequest;
  }
}

// Process the word at the start of the request, which should be the HTTP
// method name. The space following matched_text has already been removed from
// the start of view.
EHttpStatusCode ProcessHttpMethod(RequestDecoderState& state,
                                  const StringView& matched_text,
                                  StringView& view) {
  EHttpMethod method;
  if (MatchHttpMethod(matched_text, method)) {
    TAS_VLOG(3) << TAS_FLASHSTR("method: ") << method;
    state.request.http_method = method;
    return state.SetDecodeFunction(MatchStartOfPath);

  } else {
    return EHttpStatusCode::kHttpMethodNotImplemented;
  }
}

// Decode one of the few supported HTTP methods. If definitely not present,
// returns an error. We *could* allow for leading whitespace, which has been
// supported implementations in the past, perhaps to deal with multiple
// requests (or multiple responses) in a row without clear delimiters. However
// HTTP/1.1 requires clear delimiters, and we're planning to support only a
// single request per TCP connection (i.e. we won't support Keep-Alive).
EHttpStatusCode DecodeHttpMethod(RequestDecoderState& state, StringView& view) {
  return ExtractAndProcessName(
      state, view, kHttpMethodTerminators, ProcessHttpMethod,
      /*consume_terminator_char=*/true,
      /*bad_terminator_error=*/EHttpStatusCode::kHttpBadRequest);
}

}  // namespace

size_t PrintValueTo(DecodeFunction decode_function, Print& out) {
#define OUTPUT_METHOD_NAME(symbol) \
  if (decode_function == symbol) return out.print(#symbol)

  OUTPUT_METHOD_NAME(DecodeApiGroup);
  OUTPUT_METHOD_NAME(DecodeApiVersion);
  OUTPUT_METHOD_NAME(DecodeDeviceMethod);
  OUTPUT_METHOD_NAME(DecodeDeviceNumber);
  OUTPUT_METHOD_NAME(DecodeDeviceType);
  OUTPUT_METHOD_NAME(DecodeEndOfPath);
  OUTPUT_METHOD_NAME(DecodeHeaderLineEnd);
  OUTPUT_METHOD_NAME(DecodeHeaderLines);
  OUTPUT_METHOD_NAME(DecodeHeaderName);
  OUTPUT_METHOD_NAME(DecodeHeaderValue);
  OUTPUT_METHOD_NAME(DecodeHttpMethod);
  OUTPUT_METHOD_NAME(DecodeManagementMethod);
  OUTPUT_METHOD_NAME(DecodeManagementType);
  OUTPUT_METHOD_NAME(DecodeParamName);
  OUTPUT_METHOD_NAME(DecodeParamSeparator);
  OUTPUT_METHOD_NAME(DecodeParamValue);
  OUTPUT_METHOD_NAME(MatchHttpVersion);
  OUTPUT_METHOD_NAME(MatchStartOfPath);
  OUTPUT_METHOD_NAME(SkipHeaderValue);

#undef OUTPUT_METHOD_NAME

  // COV_NF_START
  TAS_CHECK(false) << TAS_FLASHSTR(
      "Haven't implemented a case for decode_function");
  return 0;
  // COV_NF_END
}

RequestDecoderState::RequestDecoderState(AlpacaRequest& request,
                                         RequestDecoderListener* listener)
    : decode_function(nullptr),
      request(request)
#if TAS_ENABLE_REQUEST_DECODER_LISTENER
      ,
      listener(listener)
#endif  // TAS_ENABLE_REQUEST_DECODER_LISTENER
{
}

void RequestDecoderState::Reset() {
  TAS_VLOG(1) << TAS_FLASHSTR(
      "Reset ################################################################");
  decode_function = DecodeHttpMethod;
  request.Reset();
  is_decoding_header = true;
  is_decoding_start_line = true;
  is_final_input = false;
  found_content_length = false;
  decoder_status = RequestDecoderStatus::kReset;
}

EHttpStatusCode RequestDecoderState::DecodeBuffer(StringView& buffer,
                                                  const bool buffer_is_full,
                                                  const bool at_end_of_input) {
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeBuffer ") << HexEscaped(buffer);
  if (decode_function == nullptr) {
    // Need to call Reset first.
    //
    // Why not call Reset automatically from the ctor? Because we assume that
    // these objects will have static storage when in the embedded system, and
    // will be used for decoding multiple requests; therefore it doesn't make
    // sense to have special behavior in the caller to omit the first call to
    // Reset.
    return EHttpStatusCode::kHttpInternalServerError;
  }

  TAS_DCHECK_NE(decoder_status, RequestDecoderStatus::kDecoded);
  if (decoder_status == RequestDecoderStatus::kReset) {
    decoder_status = RequestDecoderStatus::kDecoding;
  }

  const auto start_size = buffer.size();
  EHttpStatusCode status;
  if (is_decoding_header) {
    status = DecodeMessageHeader(buffer, at_end_of_input);
  } else {
    status = DecodeMessageBody(buffer, at_end_of_input);
  }
  TAS_DCHECK_NE(status, EHttpStatusCode::kContinueDecoding);

  if (buffer_is_full && status == EHttpStatusCode::kNeedMoreInput &&
      start_size == buffer.size()) {
    TAS_VLOG(1) << TAS_FLASHSTR(
        "Need more input, but buffer is already full "
        "(has no room for additional input).");
    status = EHttpStatusCode::kHttpRequestHeaderFieldsTooLarge;
  }
  if (status >= EHttpStatusCode::kHttpOk) {
    decode_function = nullptr;
    decoder_status = RequestDecoderStatus::kDecoded;
  }
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeBuffer --> ") << status;
  return status;
}

// Decoding the start line, header lines, or end of header line. We don't know
// how many bytes are supposed to be in the header, so we rely on
// DecodeHeaderLines to find the end.
EHttpStatusCode RequestDecoderState::DecodeMessageHeader(
    StringView& buffer, const bool at_end_of_input) {
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeMessageHeader ") << HexEscaped(buffer);

  EHttpStatusCode status;
  do {
#if TAS_ENABLE_DEBUGGING
    const auto buffer_size_before_decode = buffer.size();
    auto old_decode_function = decode_function;
    TAS_VLOG(2) << decode_function << ' ' << HexEscaped(buffer)
                << TAS_FLASHSTR(" (") << static_cast<size_t>(buffer.size())
                << TAS_FLASHSTR(" chars))");
#endif

    status = decode_function(*this, buffer);

#if TAS_ENABLE_DEBUGGING
    TAS_CHECK_LE(buffer.size(), buffer_size_before_decode);
    auto consumed_chars = buffer_size_before_decode - buffer.size();

    TAS_VLOG(3) << TAS_FLASHSTR("decode_function ") << TAS_FLASHSTR("returned ")
                << status << TAS_FLASHSTR(", consumed ") << consumed_chars
                << TAS_FLASHSTR(" characters, ")
                << TAS_FLASHSTR("decode_function ")
                << (old_decode_function == decode_function
                        ? TAS_FLASHSTR("unchanged")
                        : TAS_FLASHSTR("changed"));

    if (status == EHttpStatusCode::kContinueDecoding) {
      // This is a check on the currently expected behavior; none of the current
      // decode functions represents a loop all by itself, which isn't handled
      // inside the decode function; i.e. none of them remove some, but not all,
      // of the input from buffer, and then return kContinueDecoding without
      // also calling SetDecodeFunction to specify the next (different) function
      // to handle the decoding.
      TAS_CHECK_NE(old_decode_function, decode_function) << TAS_FLASHSTR(
          "Should have changed the decode function");  // COV_NF_LINE
    }
#endif
  } while (status == EHttpStatusCode::kContinueDecoding);

  if (status == EHttpStatusCode::kNeedMoreInput) {
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
EHttpStatusCode RequestDecoderState::DecodeMessageBody(StringView& buffer,
                                                       bool at_end_of_input) {
  TAS_VLOG(1) << TAS_FLASHSTR("DecodeMessageBody ") << HexEscaped(buffer);
  TAS_CHECK(found_content_length);
  TAS_CHECK_EQ(request.http_method, EHttpMethod::PUT);

  if (buffer.size() > remaining_content_length) {
    // We assume that the HTTP client has not sent pipelined requests.
    TAS_VLOG(2) << TAS_FLASHSTR(
                       "There is more input than Content-Length indicated: ")
                << buffer.size() << TAS_FLASHSTR(" > ")
                << remaining_content_length;
    return EHttpStatusCode::kHttpPayloadTooLarge;
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
      return EHttpStatusCode::kHttpBadRequest;
    }
  }

  EHttpStatusCode status;
  do {
    const auto buffer_size_before_decode = buffer.size();
#if TAS_ENABLE_DEBUGGING
    const auto old_decode_function = decode_function;
    TAS_VLOG(2) << decode_function << ' ' << HexEscaped(buffer)
                << TAS_FLASHSTR(" (") << (buffer.size() + 0)
                << TAS_FLASHSTR(" chars))");
#endif

    status = decode_function(*this, buffer);
    const auto consumed_chars = buffer_size_before_decode - buffer.size();

#if TAS_ENABLE_DEBUGGING
    TAS_VLOG(3) << TAS_FLASHSTR("decode_function ") << TAS_FLASHSTR("returned ")
                << status << TAS_FLASHSTR(", consumed ") << consumed_chars
                << TAS_FLASHSTR(" characters, ")
                << TAS_FLASHSTR("decode_function ")
                << (old_decode_function == decode_function
                        ? TAS_FLASHSTR("unchanged")
                        : TAS_FLASHSTR("changed"));
    TAS_CHECK_LE(buffer.size(), buffer_size_before_decode);
    TAS_CHECK_LE(consumed_chars, remaining_content_length);
    if (decode_function == old_decode_function) {
      // This is a check on the currently expected behavior; none of the current
      // decode functions represents a loop all by itself, which isn't handled
      // inside the decode function; i.e. none of them remove some, but not all,
      // of the input from buffer, and then return kContinueDecoding without
      // also calling SetDecodeFunction to specify the next (different) function
      // to handle the decoding.
      TAS_CHECK_NE(status, EHttpStatusCode::kContinueDecoding);
    }
    if (buffer_size_before_decode == 0) {
      // We don't bother checking whether the buffer is empty at the start or
      // end of the loop because it saves a little bit of code space, in
      // exchange for another pass through the loop, thus requiring a
      // DecodeFunction to notice that there isn't enough input for it to
      // succeed.
      TAS_CHECK_NE(status, EHttpStatusCode::kContinueDecoding);
    }
#endif

    remaining_content_length -= consumed_chars;
  } while (status == EHttpStatusCode::kContinueDecoding);

  TAS_CHECK_NE(status, EHttpStatusCode::kContinueDecoding);

  if (status >= EHttpStatusCode::kHttpOk) {
#if TAS_ENABLE_DEBUGGING
    if (status == EHttpStatusCode::kHttpOk) {
      TAS_CHECK_EQ(remaining_content_length, 0);
      TAS_CHECK(at_end_of_input);
    }
#endif
    return status;
  }

  TAS_CHECK_EQ(status, EHttpStatusCode::kNeedMoreInput);
  if (at_end_of_input) {
    return EHttpStatusCode::kHttpBadRequest;
  }

  TAS_CHECK_GT(remaining_content_length, 0);
  return status;
}

EHttpStatusCode RequestDecoderState::SetDecodeFunction(
    const DecodeFunction func) {
  TAS_VLOG(3) << TAS_FLASHSTR("SetDecodeFunction(") << func << ')';
  TAS_CHECK_NE(decode_function, nullptr);
  TAS_CHECK_NE(decode_function, func);
  decode_function = func;
  return EHttpStatusCode::kContinueDecoding;
}

EHttpStatusCode RequestDecoderState::SetDecodeFunctionAfterListenerCall(
    DecodeFunction func, EHttpStatusCode status) {
  TAS_CHECK_NE(status, EHttpStatusCode::kNeedMoreInput);
  if (status == EHttpStatusCode::kContinueDecoding) {
    return SetDecodeFunction(func);
  } else if (status < EHttpStatusCode::kHttpOk) {
    return EHttpStatusCode::kHttpInternalServerError;  // COV_NF_LINE
  } else {
    return status;
  }
}

}  // namespace alpaca
