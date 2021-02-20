#ifndef TINY_ALPACA_SERVER_CONFIG_H_
#define TINY_ALPACA_SERVER_CONFIG_H_

// Controls the set of features available in the decoder.

#ifdef Arduino_h
#define ALPACA_SERVER_EMBEDDED 1
#define ALPACA_SERVER_HAVE_STD_OSTREAM 0
#define ALPACA_SERVER_HAVE_STD_STRING_VIEW 0
#define ALPACA_SERVER_HAVE_STD_STRING 0
#else
#define ALPACA_SERVER_EMBEDDED 0
#define ALPACA_SERVER_HAVE_STD_OSTREAM 1
#define ALPACA_SERVER_HAVE_STD_STRING_VIEW 1
#define ALPACA_SERVER_HAVE_STD_STRING 1
#endif  // Arduino_h

#define ALPACA_SERVER_ON_HOST (!ALPACA_SERVER_EMBEDDED)

// Can't have DLOG & DCHECK macros in a constexpr function, so we use this
// macro to add the constexpr specifier when a function using those is NOT
// compiled for debugging.
#ifdef NDEBUG
#define ALPACA_SERVER_CONSTEXPR_FUNC constexpr
#define ALPACA_SERVER_CONSTEXPR_VAR constexpr
#else
#define ALPACA_SERVER_CONSTEXPR_FUNC
#define ALPACA_SERVER_CONSTEXPR_VAR const
#endif  // NDEBUG

#endif  // TINY_ALPACA_SERVER_CONFIG_H_
