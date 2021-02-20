#ifndef TINY_ALPACA_SERVER_CONFIG_H_
#define TINY_ALPACA_SERVER_CONFIG_H_

// Controls the set of features available in the Tiny Alpaca Server.
// Author: james.synge@gmail.com

#ifdef ARDUINO
#define ALPACA_SERVER_EMBEDDED 1
#define ALPACA_SERVER_HAVE_STD_OSTREAM 0
#define ALPACA_SERVER_HAVE_STD_STRING_VIEW 0
#define ALPACA_SERVER_HAVE_STD_STRING 0
#define ALPACA_SERVER_DEBUG 0
#else
#define ALPACA_SERVER_EMBEDDED 0
#define ALPACA_SERVER_HAVE_STD_OSTREAM 1
#define ALPACA_SERVER_HAVE_STD_STRING_VIEW 1
#define ALPACA_SERVER_HAVE_STD_STRING 1
#ifdef NDEBUG
#define ALPACA_SERVER_DEBUG 0
#else
#define ALPACA_SERVER_DEBUG 1
#endif  // NDEBUG
#endif  // ARDUINO

#define ALPACA_SERVER_ON_HOST (!ALPACA_SERVER_EMBEDDED)

// This isn't fully fleshed out, but the basics are there for storing the
// parameter enum and short string value of parameter types that are defined
// and have token entries in kRecognizedParameters passed
#define ALPACA_SERVER_ENABLE_EXTRA_REQUEST_PARAMETERS 0
#define ALPACA_SERVER_MAX_EXTRA_REQUEST_PARAMETERS 2
#define ALPACA_SERVER_MAX_EXTRA_REQUEST_PARAMETER_LENGTH 16

// Can't have DLOG & DCHECK macros in a constexpr function, so we use this
// macro to add the constexpr specifier when a function using those is NOT
// compiled for debugging.
#if ALPACA_SERVER_DEBUG
#define ALPACA_SERVER_CONSTEXPR_FUNC
#define ALPACA_SERVER_CONSTEXPR_VAR const
#else
#define ALPACA_SERVER_CONSTEXPR_FUNC constexpr
#define ALPACA_SERVER_CONSTEXPR_VAR constexpr
#endif  // ALPACA_SERVER_DEBUG

#endif  // TINY_ALPACA_SERVER_CONFIG_H_
