#ifndef TINY_ALPACA_SERVER_CONFIG_H_
#define TINY_ALPACA_SERVER_CONFIG_H_

// Controls the set of features available in the Tiny Alpaca Server. (This is in
// contrast to platform.h, which expresses the set of features provided by the
// platform.)
//
// This file should only define macros, and not include (and hence export)
// anything else.
//
// Author: james.synge@gmail.com

// This isn't fully fleshed out, but the basics are there for storing the
// parameter enum and short string value of parameter types that are defined
// and have token entries in kRecognizedParameters passed
#define TAS_ENABLE_EXTRA_REQUEST_PARAMETERS 0
#define TAS_MAX_EXTRA_REQUEST_PARAMETERS 2
#define TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH 16

#endif  // TINY_ALPACA_SERVER_CONFIG_H_
