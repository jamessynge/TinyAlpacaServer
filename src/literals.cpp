#include "literals.h"

// The idea here is to create small objects in RAM referencing string literals
// in flash memory (PROGMEM in AVR terms).
//
// We could go further and define an enum for each string literal, and store a
// table in PROGMEM of pointers to string literals in PROGMEM, thus reducing the
// representation in code for one of these literals to an enum value, which
// probably doesn't end up in RAM often.

#include "literal.h"
#include "platform.h"

#ifdef DEFINE_LITERAL
#undef DEFINE_LITERAL
#endif  // DEFINE_LITERAL

namespace alpaca {
namespace {
// Define string literal constants in the anonymous namespace.

#define DEFINE_LITERAL(name, literal) \
  constexpr char k##name[] PROGMEM = literal;
#include "src/literals.inc"
#undef DEFINE_LITERAL
}  // namespace

// Define corresponding Literal factory functions for each constant.

#define DEFINE_LITERAL(name, literal) \
  Literal Literals::name() { return Literal(k##name); }
#include "src/literals.inc"
#undef DEFINE_LITERAL

}  // namespace alpaca
