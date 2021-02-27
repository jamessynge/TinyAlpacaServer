#ifndef TINY_ALPACA_SERVER_SRC_ESCAPING_H_
#define TINY_ALPACA_SERVER_SRC_ESCAPING_H_

// Core of support for printing JSON strings. Characters that are not valid in
// JSON strings (e.g. Ctrl-A) are not printed.

#include "platform.h"
#include "string_view.h"

namespace alpaca {

// Print c with appropriate escaping for JSON.
size_t PrintCharJsonEscaped(Print& out, char c);

// Returns a StringView of the JSON representation of the ASCII character c.
// An empty StringView is returned if the character is not valid in JSON (e.g.
// Ctrl-A). A reference to the character is required because for characters
// that don't need escaping, the StringView is constructed as a length 1 view
// of that single character.
StringView GetCharJsonEscaped(const char& c);

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_ESCAPING_H_
