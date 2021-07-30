#ifndef TINY_ALPACA_SERVER_SRC_UTILS_BASENAME_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_BASENAME_H_

// Provides macros for storing the basename of a file path in the program text,
// i.e. the part after the final slash. For example, the __FILE__ macro has a
// string value equal to either of these:
//
//      /baz/bar/foo.cc
//      C:\baz\bar\foo.cc
//
// then TAS_BASENAME(__FILE__) will be equivalent to TAS_FLASHSTR("foo.cc").
// This is used by TAS_VLOG, TAS_CHECK, etc., to capture the file name where
// those macros are used, i.e. the location from which a message is logged.
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
namespace progmem_data {

// "Forward" declaration of an undefined type. If this appears in a compiler
// error message for the expansion of TAS_BASENAME, or a related macro, it means
// that the string literal (usually __FILE__) is too long (>= nnn).
class SourceFilePathIsTooLong;

// BasenameStrPack is used instead of ProgmemStrData when we're searching for
// slashes and the terminating NUL, so that we don't have to worry about the
// compiler spending time examining the definition of ProgmemStrData, which is a
// slightly more complex class template than is BasenameStrPack. Furthermore,
// the first template parameter is a bool indicating whether the terminating NUL
// has been found. This helps us generate a compile error if the string is
// longer than the template parameter pack expansion allowed for.
template <bool NulFound, char... C>
struct BasenameStrPack final {};

// When KeepBasenameOnly has a single parameter, the return type has the same
// type as that parameter. This is the case once we've used the specializations
// of KeepBasenameOnly to identify the location of the terminating NUL and are
// keeping only the characters before that; it is also the case when we've
// reached the end of the template parameter pack expansion, and failed to find
// the terminating NUL. In the former case, T will be true, while in the latter
// it will be false, which allows the appropriate ProvideStorage specialization
// to reject paths that are too long.
template <bool T, char... X>
auto KeepBasenameOnly(BasenameStrPack<T, X...>)  // as is...
    -> BasenameStrPack<T, X...>;

// Keep only the characters after a slash (forward for Linux, etc., backwards
// for Windows). There is no attempt here to only support slashes of the type
// used by the host operating system, we just assume that either type of slash
// marks the boundary between directory and directory entry.
template <char... X, char... Y>
auto KeepBasenameOnly(BasenameStrPack<false, X...>, BasenameStrPack<false, '/'>,
                      BasenameStrPack<false, Y>...)
    -> decltype(KeepBasenameOnly(BasenameStrPack<false, Y>()...));
template <char... X, char... Y>
auto KeepBasenameOnly(BasenameStrPack<false, X...>,
                      BasenameStrPack<false, '\\'>,
                      BasenameStrPack<false, Y>...)
    -> decltype(KeepBasenameOnly(BasenameStrPack<false, Y>()...));

// The second and subsequent args are of type BasenameStrPack<false, C>, where C
// is a char value. If the second arg is of type BasenameStrPack<false, '\0'>,
// then we've located the NUL of the literal string, and can discard it and the
// subsequent args; the type of the result is BasenameStrPack<X...>, where
// X... represents the literal character values of the literal string. At this
// point no more substitution of deduced template arguments for KeepBasenameOnly
// is performed, and we now know the return type of
// ExpandLiteralKeepBasenameOnly.
template <char... X, char... Y>
auto KeepBasenameOnly(BasenameStrPack<false, X...>,
                      BasenameStrPack<false, '\0'>,
                      BasenameStrPack<false, Y>...)
    -> BasenameStrPack<true, X...>;

// For any character A other than '\0' or a slash, append it to the template
// parameter pack X, then consider the characters following A.
template <char A, char... X, char... Y>
auto KeepBasenameOnly(BasenameStrPack<false, X...>, BasenameStrPack<false, A>,
                      BasenameStrPack<false, Y>...)
    -> decltype(KeepBasenameOnly(BasenameStrPack<false, X..., A>(),
                                 BasenameStrPack<false, Y>()...));

// Entry point for finding the NUL that terminates the string literal, and
// eliminating directory names before the file name.
template <char... C>
auto ExpandPathKeepBasename(BasenameStrPack<false, C...>)
    -> decltype(KeepBasenameOnly(BasenameStrPack<false, C>()...));

// Specialization for empty strings. In theory we shouldn't need these, but in
// practice it avoids problems.
template <char... C>
auto ExpandPathKeepBasename(BasenameStrPack<false, '\0', C...>)
    -> BasenameStrPack<true>;

// Specialization for paths that start with a slash.
template <char... C>
auto ExpandPathKeepBasename(BasenameStrPack<false, '/', C...>)
    -> decltype(ExpandPathKeepBasename(BasenameStrPack<false, C...>()));

// If we were able to find the NUL at the end of the string literal, then
// ProvideStorage will return a type that has a static array with the string in
// it.
template <char... C>
auto ProvideStorage(BasenameStrPack<true, C...>) -> ProgmemStrData<C...>;

// Else if the literal is too long for the expension macro used, ProvideStorage
// will return a type that isn't useful for our purposes below, and whose name
// hints at the problem.
template <char... C>
auto ProvideStorage(BasenameStrPack<false, C...>) -> SourceFilePathIsTooLong;

// If your paths are longer than 127 characters, change _TAS_EXPAND_128 below to
// _TAS_EXPAND_256, _TAS_EXPAND_512 or even add _TAS_EXPAND_150 to get a good
// trade-off between being long enough and not being too slow to compile.
//
// NOTE: This is horribly slow when compiled with avr-gcc and _TAS_EXPAND_256 on
// my laptop (vs on a beefy workstation). To avoid this, the macro
// _TAS_EXPAND_BASENAME is defined differently for AVR and non-AVR. The longest
// path I've seen on my laptop is around 120 characters, so I'm using
// _TAS_EXPAND_128. If the lengths become too large, there will be a compiler
// failure mentioning the undefined class "SourceFilePathIsTooLong".
//
// TODO(jamessynge): Come up with a way to speed this up, such as a recursive,
// divide-and-conquer strategy, which should result in lots of the same type
// being evaluated, rather than lots of unique types.

#ifdef ARDUINO
#define _TAS_EXPAND_BASENAME(n, x) _TAS_EXPAND_128(n, x)
#else
#define _TAS_EXPAND_BASENAME(n, x) _TAS_EXPAND_256(n, x)
#endif

#define _TAS_KEEP_BASENAME_ONLY(path_literal)                              \
  decltype(::alpaca::progmem_data::ExpandPathKeepBasename(                 \
      ::alpaca::progmem_data::BasenameStrPack<false, _TAS_EXPAND_BASENAME( \
                                                         , path_literal)>()))

#define _TAS_BASENAME_TYPE(path_literal)           \
  decltype(::alpaca::progmem_data::ProvideStorage( \
      _TAS_KEEP_BASENAME_ONLY(path_literal)()))

#define TAS_BASENAME(x) \
  (reinterpret_cast<const __FlashStringHelper*>(_TAS_BASENAME_TYPE(x)::kData))

}  // namespace progmem_data
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_BASENAME_H_
