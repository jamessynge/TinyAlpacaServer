#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PGMSPACE_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PGMSPACE_H_

// This is a minimal implementation of avr-libc's Program Space Utilities, for
// the sake limiting the number of #ifdef's in the code of Tiny Alpaca Server.
//
// Note that so far this only provides the "near pointer" functions, i.e. for
// 16-bit addresses within the first 64KB of the flash. If necessary, support
// for far pointers (uint32_t I think, of which 24 mean something) can be added.
//
// The argument names have been selected to match the documentation here:
// https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html
//
// Author: james.synge@gmail.com

#include <stdint.h>

#include <cstddef>

#define PROGMEM
#define PGM_P const char*
#define PGM_VOID_P const void*
#define PSTR(s) ((const PROGMEM char*)(s))

uint8_t pgm_read_byte_near(const uint8_t* ptr);
inline uint8_t pgm_read_byte_near(const char* ptr) {
  return pgm_read_byte_near(reinterpret_cast<const uint8_t*>(ptr));
}
#define pgm_read_byte(ptr) pgrm_read_byte_near(ptr)
uint32_t pgm_read_dword_far(const uint32_t* ptr);
int memcmp_P(const void* lhs, const void* rhs, size_t count);
int strncasecmp_P(const char* s1, const char* s2, size_t n);
void* memcpy_P(void* dest, const void* src, size_t n);
const char* strrchr_P(const char* s, int val);

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_PGMSPACE_H_
