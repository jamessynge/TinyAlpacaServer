// This file is not meant to be executed. Instead, it serves as a tool for
// measuring the program (FLASH) and globals (RAM) storage requirements of
// decoding an HTTP request with an optional body with URL encoded parameters.

#ifdef ARDUINO
#include <Arduino.h>
#include <TinyAlpacaServer.h>
#else
#include "TinyAlpacaServer.h"
#endif

using ::alpaca::AlpacaRequest;
using ::alpaca::CountingBitbucket;
using ::alpaca::JsonArrayEncoder;
using ::alpaca::JsonElementSource;
using ::alpaca::JsonObjectEncoder;
using ::alpaca::JsonPropertySource;
using ::alpaca::RequestDecoder;
using ::alpaca::RequestDecoderListener;
using ::alpaca::StringView;

// Empty sketch (with or without include of TinyAlpacaServer): 662 program
// bytes, 9 bytes of ram.

void setup() {
  // Adding initialization of Serial device: 1784/184 program/ram bytes
  Serial.begin(9600);  // start serial port at 9600 bps:
  while (!Serial) {
  }  // wait for serial port to connect. Needed for native USB port only

  reset_decoder();
}

void loop() { handle_input(); }

// Add unreferenced listener instance: no change
RequestDecoderListener listener;

// Add unreferenced request instance: no change.
AlpacaRequest request;

// Add decoder instance, referencing the other two: 2456/242 (i.e. +672/+58)
RequestDecoder decoder(request, listener);

// Add call to reset_decoder (hence to RequestDecoder::Request): 6654/768 (i.e.
// +4198/+526) Calling Reset() likely brought in all of the DecodeFunctions
// because the first of them is referenced by Reset, and they're chained
// together. RAM rose so much because of all the literal strings that get copied
// into RAM in order to be used.
void reset_decoder() { decoder.Reset(); }

// Adding request_data_buffer: no change (because unreferenced?)
char request_data_buffer[64];

// Calling empty handle_input from loop: no change
void handle_input() {
  // Adding buffer_view: no change (because unreferenced?)
  StringView buffer_view(request_data_buffer, 33);

  // Calling remove_prefix: 6694/834.
  buffer_view.remove_prefix(1);

  // Adding call to to_uint32: 6712/834
  uint32_t out;
  buffer_view.to_uint32(out);

  // Adding call to DecodeBuffer: 6902/834
  decoder.DecodeBuffer(buffer_view, false, false);
}
