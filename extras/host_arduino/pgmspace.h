#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PGMSPACE_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PGMSPACE_H_

// This is a minimal implementation of avr-libc's Program Space Utilities, for
// the sake limiting the number of #ifdef's in the code of Tiny Alpaca Server.
//
// Note that so far this only works with "near pointers", i.e. 16-bit addresses
// within the first 64KB of the flash. If necessary, far pointer support can
// be added.
//
// The argument names have been selected to match the documentation here:
// https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html
//
// Author: james.synge@gmail.com

#include <cstddef>
#include <cstdint>

#define PROGMEM
#define PGM_P const char*
#define PGM_VOID_P const void*

namespace alpaca {
uint8_t pgm_read_byte(const uint8_t* ptr);
int memcmp_P(const void* lhs, const void* rhs, size_t count);
int strncasecmp_P(const char* s1, const char* s2, size_t n);
void* memcpy_P(void* dest, const void* src, size_t n);
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PGMSPACE_H_
