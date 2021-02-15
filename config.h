#ifndef ALPACA_DECODER_CONFIG_H_
#define ALPACA_DECODER_CONFIG_H_

// Controls the set of features available in the decoder.

#define ALPACA_DECODER_HAVE_STD_OSTREAM 1
#define ALPACA_DECODER_HAVE_STD_STRING_VIEW 1
#define ALPACA_DECODER_HAVE_STD_STRING \
  (1 && ALPACA_DECODER_HAVE_STD_STRING_VIEW)

// Can't have DLOG & DCHECK macros in a constexpr function, so we use this
// macro to add the constexpr specifier when a function using those is NOT
// compiled for debugging.
#ifdef NDEBUG
#define ALPACA_CONSTEXPR_FUNC constexpr
#define ALPACA_CONSTEXPR_VAR constexpr
#else
#define ALPACA_CONSTEXPR_FUNC
#define ALPACA_CONSTEXPR_VAR const
#endif  // NDEBUG

#endif  // ALPACA_DECODER_CONFIG_H_
