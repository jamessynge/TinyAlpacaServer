#include "extras/host_arduino/character.h"

#include "absl/strings/ascii.h"

bool isPrintable(const char c) { return absl::ascii_isprint(c); }
bool isAlphaNumeric(const char c) { return absl::ascii_isalnum(c); }
bool isUpperCase(const char c) { return absl::ascii_isupper(c); }
