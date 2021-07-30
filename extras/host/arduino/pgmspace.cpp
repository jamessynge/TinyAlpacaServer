#include "extras/host/arduino/pgmspace.h"

#include <strings.h>

#include <cstring>

#include "absl/strings/string_view.h"

uint8_t pgm_read_byte_near(const uint8_t* ptr) { return *ptr; }

uint32_t pgm_read_dword_far(const uint32_t* ptr) { return *ptr; }

int memcmp_P(const void* lhs, const void* rhs, size_t count) {
  return std::memcmp(lhs, rhs, count);
}

int strncasecmp_P(const char* s1, const char* s2, size_t n) {
  // Note that strncasecmp is NOT a part of the C or C++ standard libraries,
  // though it is a part of POSIX.
  return strncasecmp(s1, s2, n);
}

void* memcpy_P(void* dest, const void* src, size_t n) {
  return memcpy(dest, src, n);
}

const char* strrchr_P(const char* s, int val) { return strrchr(s, val); }
