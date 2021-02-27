#include "token_tables.h"

// TODO(jamessynge): Describe why this file exists/what it provides.

#include "decoder_constants.h"
#include "literal.h"
#include "literal_token.h"
#include "literals.h"
#include "platform.h"

namespace alpaca {

namespace {

#define MAKE_HTTP_METHOD_LITERAL_TOKEN(method)                       \
  LiteralToken<EHttpMethod_UnderlyingType> {                         \
    Literal(progmem::k##method),                                     \
        static_cast<EHttpMethod_UnderlyingType>(EHttpMethod::method) \
  }

TAS_CONSTEXPR_VAR LiteralToken<EHttpMethod_UnderlyingType>
    kRecognizedHttpMethods[] = {
        MAKE_HTTP_METHOD_LITERAL_TOKEN(GET),
        MAKE_HTTP_METHOD_LITERAL_TOKEN(PUT),
        MAKE_HTTP_METHOD_LITERAL_TOKEN(HEAD),
};

}  // namespace

bool MatchHttpMethod(const StringView& view, EHttpMethod& match) {
  auto* match_ptr = reinterpret_cast<EHttpMethod_UnderlyingType*>(&match);
  return MaybeMatchLiteralTokensExactly(view, kRecognizedHttpMethods,
                                        *match_ptr);
}

}  // namespace alpaca
