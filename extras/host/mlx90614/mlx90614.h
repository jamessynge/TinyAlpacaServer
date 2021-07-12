#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_MLX90614_MLX90614_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_MLX90614_MLX90614_H_

// Minimal fake version of SparkFun's MLX90614 library.

#include "extras/host/arduino/arduino.h"

typedef enum { TEMP_RAW, TEMP_K, TEMP_C, TEMP_F } temperature_units;

class IRTherm {
 public:
  IRTherm() {}
  bool begin() { return true; }
  bool isConnected() { return true; }
  void setUnit(temperature_units unit) {}
  bool read() { return true; }
  float object() { return -10.0; }
  float ambient() { return 10; }

  // readID() reads the 64-bit ID of the MLX90614.
  // Return value is either 1 on success or 0 on failure.
  uint8_t readID() { return 1; }

  // After calling readID() getIDH() and getIDL() can be called to read
  // the upper 4 bytes and lower 4-bytes, respectively, of the MLX90614's
  // identification registers.
  uint32_t getIDH() { return 0x12345678; }
  uint32_t getIDL() { return 0x98765432; }
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_MLX90614_MLX90614_H_
