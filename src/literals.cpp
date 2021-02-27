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
// Define a Literal factory function for each literal string.

#define DEFINE_LITERAL(name, literal) \
  Literal Literals::name() { return Literal(progmem::k##name); }
#include "literals.inc"
#undef DEFINE_LITERAL

}  // namespace alpaca
