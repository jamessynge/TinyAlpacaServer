#include "extras/host/arduino/pgmspace.h"

#include <cstring>

#include "absl/strings/case.h"
#include "absl/strings/string_view.h"

uint8_t pgm_read_byte(const uint8_t* ptr) { return *ptr; }

int memcmp_P(const void* lhs, const void* rhs, size_t count) {
  return std::memcmp(lhs, rhs, count);
}

int strncasecmp_P(const char* s1, const char* s2, size_t n) {
  absl::string_view v1(s1);
  absl::string_view v2(s2);
  if (n < v1.size()) {
    v1 = v1.substr(0, n);
  }
  if (n < v2.size()) {
    v2 = v2.substr(0, n);
  }
  return CaseCompare(v1, v2);
}

void* memcpy_P(void* dest, const void* src, size_t n) {
  return memcpy(dest, src, n);
}
