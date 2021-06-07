#ifndef TINY_ALPACA_SERVER_SRC_CONFIG_H_
#define TINY_ALPACA_SERVER_SRC_CONFIG_H_

// Controls the set of features available in the Tiny Alpaca Server. (This is in
// contrast to platform.h, which expresses the set of features provided by the
// platform.)
//
// This file should only define macros (and possibly constants), and not include
// (and hence export) anything else.
//
// Author: james.synge@gmail.com

// The number of hardware sockets we'll dedicate to listening for TCP
// connections to the Tiny Alpaca Server.
#define TAS_NUM_SERVER_CONNECTIONS 1

// If non-zero, RequestDecoder will make calls to the provided listener for
// query parameters and headers that it doesn't natively handle.
#define TAS_ENABLE_REQUEST_DECODER_LISTENER 0


#define SERVER_CONNECTION_INPUT_BUFFER_SIZE 34

// This isn't fully fleshed out, but the basics are there for storing the
// parameter enum and short string value of parameter types that are defined
// and have token entries in kRecognizedParameters passed
#define TAS_ENABLE_EXTRA_REQUEST_PARAMETERS 1
#define TAS_MAX_EXTRA_REQUEST_PARAMETERS 2
#define TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH 34

#endif  // TINY_ALPACA_SERVER_SRC_CONFIG_H_
