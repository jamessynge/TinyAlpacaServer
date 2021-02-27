#ifndef TINY_ALPACA_SERVER_SRC_LITERALS_H_
#define TINY_ALPACA_SERVER_SRC_LITERALS_H_

// To avoid wasting RAM on string literals on Arduino's based on Microchip
// Techonology's AVR microcontrollers, we use the Literal class to access those
// strings from PROGMEM (i.e. Program Memory, Flash). To hide some of the
// complexities, we expose accessor methods which return Literal instances; this
// has the benefit that we don't occupy RAM for the Literal instances, though at
// the expense of more small functions for returning the Literals.

#include "literal.h"
#include "platform.h"

#ifdef DEFINE_LITERAL
#undef DEFINE_LITERAL
#endif  // DEFINE_LITERAL

namespace alpaca {
// Define string literal constants in an inner progmem namespace.
namespace progmem {
#define DEFINE_LITERAL(name, literal) \
  constexpr char k##name[] PROGMEM = literal;
#include "literals.inc"
#undef DEFINE_LITERAL
}  // namespace progmem

#define DEFINE_LITERAL(name, literal) static Literal name();

struct Literals {
#include "literals.inc"
};

#undef DEFINE_LITERAL

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_LITERALS_H_
