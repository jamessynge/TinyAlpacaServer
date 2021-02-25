#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_LITERALS_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_LITERALS_H_

// The idea here is to create small objects in RAM referencing string literals
// in flash memory (PROGMEM in AVR terms).
//
// We could go further and define an enum for each string literal, and store a
// table in PROGMEM of pointers to string literals in PROGMEM, thus reducing the
// representation in code for one of these literals to an enum value, which
// probably doesn't end up in RAM often.

#include "src/literal.h"
#include "src/platform.h"

namespace alpaca {

namespace lc_string {
#define DEFINE_LITERAL(name, literal) constexpr char name[] PROGMEM = literal;

#include "extras/futures/lower_case_literals.inc"

#undef DEFINE_LITERAL
}  // namespace lc_string

namespace lc_literal {

#define DEFINE_LITERAL(name, literal) \
  constexpr Literal name(::alpaca::lc_string::name);

#include "extras/futures/lower_case_literals.inc"

#undef DEFINE_LITERAL

}  // namespace lc_literal
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_LITERALS_H_
