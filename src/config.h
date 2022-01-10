#ifndef TINY_ALPACA_SERVER_SRC_CONFIG_H_
#define TINY_ALPACA_SERVER_SRC_CONFIG_H_

// Controls the set of features available in the Tiny Alpaca Server.
//
// See mcucore_config.h for config related to logging, etc., and
// mcucore_platform.h for settings/includes related to the platform on which the
// server runs.
//
// This file should only define macros (and possibly constants), and not include
// (and hence export) anything else.
//
// Author: james.synge@gmail.com

// The number of hardware sockets we'll dedicate to listening for TCP
// connections to the Tiny Alpaca Server.
#define TAS_NUM_SERVER_CONNECTIONS 3

// If non-zero, RequestDecoder will make calls to the provided listener for
// query parameters and headers that it doesn't natively handle.
#define TAS_ENABLE_REQUEST_DECODER_LISTENER 0

// Number of bytes for storage of incoming request bytes. This needs to be 1
// byte larger than the largest item that we will need to be able to match,
// where that extra byte is necessary to detect the end of that item.
#define SERVER_CONNECTION_INPUT_BUFFER_SIZE 128

// This isn't fully fleshed out, but the basics are there for storing the
// parameter enum and short string value of parameter types that are defined
// and have token entries in kRecognizedParameters passed
#define TAS_ENABLE_EXTRA_REQUEST_PARAMETERS 0
#define TAS_MAX_EXTRA_REQUEST_PARAMETERS 2
#define TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH 128

#endif  // TINY_ALPACA_SERVER_SRC_CONFIG_H_
