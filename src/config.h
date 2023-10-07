#ifndef TINY_ALPACA_SERVER_SRC_CONFIG_H_
#define TINY_ALPACA_SERVER_SRC_CONFIG_H_

// Sets defaults for the set of features available in the Tiny Alpaca Server.
// Many of these can be overridden on the compiler command line.
//
// See mcucore_config.h for config related to logging, etc., and
// mcucore_platform.h for settings/includes related to the platform on which the
// server runs.
//
// This file should only define macros (and possibly constants). To ensure that
// mcucore_platform.h is included, we include it here, but don't include (hence
// don't export symbols from) anything other files.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#ifndef REQUEST_DECODER_DISABLE_EXTRA_CHECKS
#if MCU_HOST_TARGET && MCU_ENABLE_DEBUGGING
#define REQUEST_DECODER_EXTRA_CHECKS
#endif
#endif

#ifndef TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES
#if MCU_HOST_TARGET && !defined(NDEBUG)
#define TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES 1
#else
#define TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES 0
#endif
#endif

// The number of hardware sockets we'll dedicate to listening for TCP
// connections to the Tiny Alpaca Server.
#ifndef TAS_NUM_SERVER_CONNECTIONS
#define TAS_NUM_SERVER_CONNECTIONS 2
#endif

// If non-zero, RequestDecoder will make calls to the OnAssetPathSegment method
// of the RequestDecoderListener, if provided. If zero, then the method is not
// defined, so there is no space taken up for (stub) implementations of the
// method, nor in the vtable.
#ifndef TAS_ENABLE_ASSET_PATH_DECODING
#define TAS_ENABLE_ASSET_PATH_DECODING 1
#endif

// If non-zero, RequestDecoder will make calls to the OnExtraParameter method
// of the RequestDecoderListener, if provided. If zero, then the method is not
// defined, so there is no space taken up for (stub) implementations of the
// method, nor in the vtable.
#ifndef TAS_ENABLE_EXTRA_PARAMETER_DECODING
#define TAS_ENABLE_EXTRA_PARAMETER_DECODING \
  TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES
#endif

// If non-zero, RequestDecoder will make calls to the OnUnknownParameterName and
// OnUnknownParameterValue methods of the RequestDecoderListener, if provided.
// If zero, then the methods are not defined, so there is no space taken up for
// (stub) implementations of the methods, nor in the vtable.
#ifndef TAS_ENABLE_UNKNOWN_PARAMETER_DECODING
#define TAS_ENABLE_UNKNOWN_PARAMETER_DECODING \
  TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES
#endif

// If non-zero, RequestDecoder will make calls to the OnExtraHeader method
// of the RequestDecoderListener, if provided. If zero, then the method is not
// defined, so there is no space taken up for (stub) implementations of the
// method, nor in the vtable.
#ifndef TAS_ENABLE_EXTRA_HEADER_DECODING
#define TAS_ENABLE_EXTRA_HEADER_DECODING \
  TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES
#endif

// If non-zero, RequestDecoder will make calls to the OnUnknownHeaderName and
// OnUnknownHeaderValue methods of the RequestDecoderListener, if provided.
// If zero, then the methods are not defined, so there is no space taken up for
// (stub) implementations of the methods, nor in the vtable.
#ifndef TAS_ENABLE_UNKNOWN_HEADER_DECODING
#define TAS_ENABLE_UNKNOWN_HEADER_DECODING \
  TAS_ENABLE_TESTING_OF_ALL_REQUEST_DECODER_LISTENER_FEATURES
#endif

// Number of bytes for storage of incoming request bytes. This needs to be 1
// byte larger than the largest item that we will need to be able to match,
// where that extra byte is necessary to detect the end of that item.
#ifndef SERVER_CONNECTION_INPUT_BUFFER_SIZE
#define SERVER_CONNECTION_INPUT_BUFFER_SIZE 255
#endif

// This isn't fully fleshed out, but the basics are there for storing the
// parameter enum and short string value of parameter types that are defined
// and have token entries in kRecognizedParameters passed
#define TAS_ENABLE_EXTRA_REQUEST_PARAMETERS 0
#define TAS_MAX_EXTRA_REQUEST_PARAMETERS 2
#define TAS_MAX_EXTRA_REQUEST_PARAMETER_LENGTH 128

#endif  // TINY_ALPACA_SERVER_SRC_CONFIG_H_
