#include "literals.h"

namespace alpaca {

// Define string literal constants in a nested namespace.
namespace progmem_data {
#define TAS_DEFINE_BUILTIN_LITERAL(name, literal) \
  constexpr char k##name[] AVR_PROGMEM = literal;
#include "literals.inc"
#undef TAS_DEFINE_BUILTIN_LITERAL
}  // namespace progmem_data

// Define static mcucore::Literal factory methods in a struct, acting as a
// nested namespace.
#define TAS_DEFINE_BUILTIN_LITERAL(name, literal)   \
  mcucore::Literal Literals::name() {               \
    return mcucore::Literal(progmem_data::k##name); \
  }

#include "literals.inc"

#undef TAS_DEFINE_BUILTIN_LITERAL

}  // namespace alpaca
