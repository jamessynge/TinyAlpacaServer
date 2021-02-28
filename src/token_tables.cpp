#include "token_tables.h"

// Implements lookups mapping a StringView to an enum, by way of
// matching the StringView agains a table of LiteralTokens.

#include "decoder_constants.h"
#include "literal.h"
#include "literal_token.h"
#include "literals.h"
#include "platform.h"

namespace alpaca {

namespace {

#define MAKE_ENUM_LITERAL_TOKEN(enum_type, literal_name, enum_value)                       \
  LiteralToken<enum_type ## _UnderlyingType> {                         \
    Literal(progmem::k ## literal_name),                                     \
        static_cast<enum_type ## _UnderlyingType>(enum_type::enum_value) \
  }

#define MAKE_HTTP_METHOD_LITERAL_TOKEN(method)                       \
  MAKE_ENUM_LITERAL_TOKEN(EHttpMethod, method, method)

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
