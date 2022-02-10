#ifndef TINY_ALPACA_SERVER_SRC_LITERALS_H_
#define TINY_ALPACA_SERVER_SRC_LITERALS_H_

// To avoid wasting RAM on string literals on Arduino's based on Microchip
// Techonology's AVR microcontrollers, we gather the strings together in
// literals.inc, and then include that file here to define arrays in PROGMEM for
// those strings, and to define factory functions for each string, one returing
// a  ProgmemStringView, another a ProgmemString instance. The former are useful
// when you need to perform operations such as comparing with a StringView, the
// latter are well suited to printing (e.g. via alpaca::WriteResponse).
//
// We declare the functions as static member functions in a struct. This helps
// to detect when we've implemented a member function but failed to add it to
// the struct definition.
//
// Author: james.synge@gmail.com

#include "mcucore_platform.h"
#include "progmem_string.h"
#include "progmem_string_view.h"

#ifdef TAS_DEFINE_PROGMEM_LITERAL
#undef TAS_DEFINE_PROGMEM_LITERAL
#endif  // TAS_DEFINE_PROGMEM_LITERAL

namespace alpaca {

// Define static mcucore::ProgmemStringView factory methods in struct
// ProgmemStringViews, acting as a nested namespace, but ensuring that each
// method defined in the source file matches a declaration in the header file
// (i.e. if )
struct ProgmemStringViews {
#define TAS_DEFINE_PROGMEM_LITERAL(name, literal) \
  static mcucore::ProgmemStringView name();

#include "literals.inc"  // IWYU pragma: export

#undef TAS_DEFINE_PROGMEM_LITERAL
};

// Define static mcucore::ProgmemString factory methods in struct
// ProgmemStringss, acting as a nested namespace, but ensuring that each
// method defined in the source file matches a declaration in the header file.
struct ProgmemStrings {
#define TAS_DEFINE_PROGMEM_LITERAL(name, literal) \
  static mcucore::ProgmemString name();

#include "literals.inc"  // IWYU pragma: export

#undef TAS_DEFINE_PROGMEM_LITERAL
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_LITERALS_H_
