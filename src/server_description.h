#ifndef TINY_ALPACA_SERVER_SRC_SERVER_DESCRIPTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_DESCRIPTION_H_

// ServerDescription provides the info used to respond to GET requests with path
// /management/v1/description.
//
// Author: james.synge@gmail.com

#include "utils/json_encoder.h"
#include "utils/literal.h"
#include "utils/platform.h"
#include "utils/string_view.h"

namespace alpaca {

// TODO(jamessynge): Allow the caller to provide either a Literal for the /setup
// HTML response, or to provide a function for that purpose, maybe as a
// Printable instance.
struct ServerDescription {
  // Write the description of this server to the specified JsonObjectEncoder.
  // The encoder should be for the nested object that is the value of the
  // "Value" property of the response object, NOT the outermost object that is
  // the body of the response to /man
  void AddTo(JsonObjectEncoder& object_encoder) const;

  // The device or server's overall name.
  Literal server_name;

  // The manufacturer's name.
  Literal manufacturer;

  // The device or server's version number.
  Literal manufacturer_version;

  // The device or server's location.
  Literal location;

#if 0
  // TODO(jamessynge): Implement support for recording the server's physical
  // location (e.g. the Lat-Long of the observatory) in EEPROM, and fetching it
  // here. Ideas for the API:

  // Returns the number of bytes used to store the saved location of the server.
  // Returns 0 if it is not stored.
  size_t StoredLocationSize() const;

  // Fill buffer with the saved location of the server, if the buffer is large
  // enough, returning the number of bytes copied. If buffer_size is not big
  // enough, returns 0. If the value is not stored in EEPROM, 0 is returned.
  size_t LoadLocation(uint8_t* buffer, size_t buffer_size) const;

  // Store the location in eeprom. Definitely NOT the final API. I really need
  // to build on my earlier EepromIO ideas:
  // https://github.com/jamessynge/arduino_experiments/blob/master/utilities/eeprom_io.h
  // In particular, the idea of having "Persistable" objects, similar to a
  // Printable object (already a part of the Arduino API).
  size_t SaveLocation(const StringView& location) const;
#endif
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_DESCRIPTION_H_
