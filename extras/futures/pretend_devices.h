#ifndef TINY_ALPACA_SERVER_EXTRAS_FUTURES_PRETEND_DEVICES_H_
#define TINY_ALPACA_SERVER_EXTRAS_FUTURES_PRETEND_DEVICES_H_

// TODO(jamessynge): Describe why this file exists/what it provides.

struct Dht22Device {
  float get_relative_humidity() const { return 50; }
  float get_temperature() const { return 45; }
};

struct AagDevice {
  float get_ambient_temperature() const { return 45; }
  float get_rain_rate() const { return 0; }
  float get_relative_humidity() const { return 50; }
  float get_sky_brightness() const { return 45; }
  float get_sky_temp() const { return 0; }
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_FUTURES_PRETEND_DEVICES_H_
