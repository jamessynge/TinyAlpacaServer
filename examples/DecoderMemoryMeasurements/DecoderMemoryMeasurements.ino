// This file is not meant to be executed. Instead, it serves as a tool for
// measuring the program (FLASH) and globals (RAM) storage requirements of
// decoding an HTTP request with an optional body with URL encoded parameters.

#include <Arduino.h>
#include <TinyAlpacaServer.h>

using ::alpaca::AlpacaRequest;
using ::alpaca::RequestDecoder;
using ::alpaca::RequestDecoderListener;
using ::mcucore::StringView;

// NOLINTBEGIN

// Empty sketch (with or without include of TinyAlpacaServer): 662 program
// bytes, 9 bytes of ram.

void setup() {
  // Adding initialization of Serial device: 1784/184 program/ram bytes
  Serial.begin(115200);  // start serial port at 9600 bps:

  // Wait for serial port to connect: 1784/184 program/ram bytes
  // (needed for native USB port only)
  while (!Serial) {
  }
}

// Add unreferenced listener instance: no change
RequestDecoderListener listener;  // NOLINT

// Add unreferenced request instance: no change.
AlpacaRequest request;

// Add decoder instance, referencing the other two: 2456/247 (i.e. +672/+63)
RequestDecoder decoder(request, &listener);

// Adding request_data_buffer: no change (because unreferenced)
char request_data_buffer[64];

void loop() {
  // Adding buffer_view: no change (because unreferenced?)
  mcucore::StringView buffer_view(request_data_buffer, 33);

  // Calling remove_prefix: 2456/247 (unreferenced?)
  buffer_view.remove_prefix(1);

  // Add call to reset_decoder (hence to RequestDecoder::Request): 7970/583
  // (i.e. +4198/+526)
  //
  // Calling Reset() likely brought in all of the DecodeFunctions because the
  // first of them is referenced by Reset, and they're chained together. RAM
  // rose so much because of all the literal strings that get copied into RAM in
  // order to be used.
  decoder.Reset();

  // Adding call to to_uint32: 7988/583
  uint32_t out;
  buffer_view.to_uint32(out);

  // Adding call to DecodeBuffer: 8178/583
  decoder.DecodeBuffer(buffer_view, false, false);
}

// NOLINTEND
