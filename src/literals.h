#ifndef TINY_ALPACA_SERVER_SRC_LITERALS_H_
#define TINY_ALPACA_SERVER_SRC_LITERALS_H_

// To avoid wasting RAM on string literals on Arduino's based on Microchip
// Techonology's AVR microcontrollers, we gather the strings together in
// literals.inc, and then include that file here to define arrays in PROGMEM for
// those strings, and to define factory functions for corresponding Literal
// instances.

#include "literal.h"
#include "platform.h"

#ifdef TAS_DEFINE_BUILTIN_LITERAL
#undef TAS_DEFINE_BUILTIN_LITERAL
#endif  // TAS_DEFINE_BUILTIN_LITERAL

namespace alpaca {
// Define string literal constants in a nested namespace.
namespace progmem_data {
#define TAS_DEFINE_BUILTIN_LITERAL(name, literal) \
  constexpr char k##name[] PROGMEM = literal;
#include "literals.inc"
#undef TAS_DEFINE_BUILTIN_LITERAL
}  // namespace progmem_data

// Define static Literal factory methods in a struct, acting as a nested
// namespace.
#define TAS_DEFINE_BUILTIN_LITERAL(name, literal)   \
  inline static TAS_CONSTEXPR_FUNC Literal name() { \
    return Literal(progmem_data::k##name);          \
  }

struct Literals {
#include "literals.inc"
};

#undef TAS_DEFINE_BUILTIN_LITERAL

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_LITERALS_H_
