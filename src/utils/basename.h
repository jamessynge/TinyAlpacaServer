#ifndef TINY_ALPACA_SERVER_SRC_UTILS_BASENAME_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_BASENAME_H_

// Provides macros for storing the basename of a file path in the program text,
// i.e. the part after the final slash. For example, storing the string "foo.cc"
// if __FILE__ is used as the path, and __FILE__ equals "/baz/bar/foo.cc". There
// is one exception: if the original path has a single slash in it, and the
// slash is at the start of the path, then the slash is retained.
//
// If compiled with avr-gcc for the AVR line of processors, the linker will
// collapse multiple occurrences of the same string literal into a single array
// in PROGMEM; this isn't true when using the Arduino defined F(path_string)
// macro, where every occurrence in a single file is stored separately.
//
// TAS_BASENAME(path_string) expands to a const __FlashStringHelper*, which when
// printed with Arduino's Print class (e.g. Serial), the basename of path_string
// is the value that is printed.
//
// Why is this useful? Because it allows TAS_CHECK (and similar macros) to
// include the name of the file from which a message is logged.
//
// This is based on inline_literal.h in the same directory as this header, which
// in turn is based on https://github.com/irrequietus/typestring, by George
// Makrydakis <george@irrequietus.eu>.
//
// Author: james.synge@gmail.com

#include "utils/inline_literal.h"
#include "utils/platform.h"

namespace alpaca {
namespace tas_basename {

template <char... C>
struct BasenameStorage final {
  static constexpr const __FlashStringHelper* FlashStringHelper() {
    return reinterpret_cast<const __FlashStringHelper*>(kData);
  }

  // We add a trailing NUL here so that we can return kData from
  // FlashStringHelper() above; Arduino's Print::print(const
  // __FlashStringHelper*) needs the string to be NUL terminated so that it
  // knows when it has found the end of the string.
  static constexpr char const kData[sizeof...(C) + 1] AVR_PROGMEM = {C..., 0};
};

template <char... C>
constexpr char const BasenameStorage<C...>::kData[sizeof...(C) + 1] AVR_PROGMEM;

// When basename_only has a single parameter, the return type has the same type
// as that parameter.
template <char... X>
auto basename_only(BasenameStorage<X...>)  // as is...
    -> BasenameStorage<X...>;

// Keep only the characters after the slash.
template <char... X, char... Y>
auto basename_only(BasenameStorage<X...>, BasenameStorage<'/'>,
                   BasenameStorage<Y>...)
    -> decltype(basename_only(BasenameStorage<Y>()...));

// Keep only the characters before the NUL.
template <char... X, char... Y>
auto basename_only(BasenameStorage<X...>, BasenameStorage<'\0'>,
                   BasenameStorage<Y>...) -> BasenameStorage<X...>;

// For any other character A, append it to the template parameter pack X, then
// consider what to do with the characters following A.
template <char A, char... X, char... Y>
auto basename_only(BasenameStorage<X...>, BasenameStorage<A>,
                   BasenameStorage<Y>...)
    -> decltype(basename_only(BasenameStorage<X..., A>(),
                              BasenameStorage<Y>()...));

// repack_basename is the "entry point"; it is used to split the output of
// TASLITnnn into nnn separate chars, and to then use basename_only to find the
// rightmost '/' and the leftmost NUL.
template <char... C>
auto repack_basename(BasenameStorage<C...>)
    -> decltype(basename_only(BasenameStorage<C>()...));

}  // namespace tas_basename
}  // namespace alpaca

// If your paths are longer than 127 characters, change TASLIT128 below to
// TASLIT256, TASLIT512 or even add TASLIT1024, if necessary.
//
// NOTE: This is horribly slow when compiled with avr-gcc and TASLIT256. To
// avoid this, the macro TAS_BASENAME_LITnnn is defined differently for AVR and
// non-AVR. The longest path I've seen on my laptop is around 120 characters, so
// I'm risking using TASLIT128.
//
// TODO(jamessynge): Come up with a way to speed this up, such as a recursive,
// divide-and-conquer strategy, which should result in lots of the same type
// being evaluated, rather than lots of unique types.
//
// TODO(jamessynge): Come up with a way to generate a static_assert or
// substitution failure in the event that the path_literal is longer than the
// TASLITnnn allows for. For example, a template function whose return type is a
// class template with a value that is the length of the path_literal.

#ifdef ARDUINO
#define TAS_BASENAME_LITnnn(n, x) TASLIT128(n, x)
#else
#define TAS_BASENAME_LITnnn(n, x) TASLIT256(n, x)
#endif

#define TAS_BASENAME_TYPE(path_literal)                            \
  decltype(::alpaca::tas_basename::repack_basename(                \
      ::alpaca::tas_basename::BasenameStorage<TAS_BASENAME_LITnnn( \
          , path_literal)>()))

#define TAS_BASENAME(path_literal) \
  (TAS_BASENAME_TYPE(path_literal)::FlashStringHelper())

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_BASENAME_H_
