// This file is not meant to be executed. Instead, it serves as a tool for
// measuring the program (FLASH) and globals (RAM) storage requirements of
// producing an HTTP response with a JSON body using JsonObjectEncoder.

#include <Arduino.h>
#include <TinyAlpacaServer.h>

using ::alpaca::AlpacaRequest;
using ::alpaca::CountingBitbucket;
using ::alpaca::JsonMethodResponse;
using ::alpaca::JsonObjectEncoder;
using ::alpaca::JsonPropertySource;

// Empty sketch (with or without include of TinyAlpacaServer): 662 program
// bytes, 9 bytes of ram.

void setup() {
  // Adding initialization of Serial device: 1784/184 program/ram bytes
  Serial.begin(9600);  // start serial port at 9600 bps:
  while (!Serial) {
  }  // wait for serial port to connect. Needed for native USB port only

  // Add writing of an empty byte array with no string storage: 1800/184.
  Serial.write(static_cast<const uint8_t *>(nullptr), 0);

  // Add writing of an empty string with no string storage: 1816/184.
  Serial.write(static_cast<const char *>(nullptr), 0);

  // Add printing of an empty string with no string storage: 1816/184 (no
  // change) It appears that this function call is inlined, whereby the compiler
  // determines that the call is a no-op (I added dozens of such calls, and
  // there was no change in program or ram size).
  Serial.print(static_cast<const char *>(nullptr));

  // Add printing of an empty string with storage: 1834/186
  const char *empty = "";
  Serial.print(empty);
  // Additional call with the same args: 1850/186
  Serial.print(empty);
}

// If we mark the string as PROGMEM, it will not be copied into RAM,
// but instead will be accessed from program storage (i.e. flash).
// Accessing from program storage takes more code, but saves on RAM.
// Note that the size of the program goes up 2 bytes at a time (i.e.
// there may be a byte of padding if the actual size is an odd number).
constexpr char abc[] AVR_PROGMEM = "123";

uint32_t MeasureCommonJsonResponseSize();
void PrintCommonHTTPResponse();

void loop() {
  // Write the first 3 bytes of a 4 byte literal stored PROGMEM: 1870/186
  Serial.write(abc, sizeof(abc) - 1);

  MeasureCommonJsonResponseSize();
  PrintCommonHTTPResponse();
}

// Add unreferenced request instance: no change (1870/186)
AlpacaRequest request;

// Add unreferenced uint32_t: no change (1870/186)
uint32_t server_transaction_id = 0;

// Define method for measuring the size of a JSON object: No change in memory.
uint32_t MeasureJsonSize(JsonPropertySource &source) {
  CountingBitbucket out;
  JsonObjectEncoder::Encode(source, out);
  return out.count();
}

// Calling this from loop: 3648/298
uint32_t MeasureCommonJsonResponseSize() {
  JsonMethodResponse source(request);
  return MeasureJsonSize(source);
}

// Calling this from loop: 3860/380
// Lots of RAM used for the literals.
void PrintCommonHTTPResponse() {
  auto size = MeasureCommonJsonResponseSize();
  Serial.print("200 OK\r\n");
  Serial.print("Connection: close\r\n");
  Serial.print("Content-Type: application/json\r\n");
  Serial.print("Content-Length: ");
  Serial.print(size);
  Serial.print("\r\n\r\n");

  // Adding in the writing of the JSON body: 3962/380
  JsonMethodResponse source(request);
  JsonObjectEncoder::Encode(source, Serial);
}
