#ifndef TINY_ALPACA_SERVER_SRC_SERVER_DESCRIPTION_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_DESCRIPTION_H_

#include "json_encoder.h"
#include "literal.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {

// There should be exactly one instance of ServerDescription in a sketch.
struct ServerDescription {
  // I would prefer that caller use C++ 20's designated initializers because it
  // is easier to understand than a constructor arg list that has no names for
  // the args (i.e. much like using keyword args in python is clearer than a
  // list of positional args). For now though, we need to use C++ 11 features to
  // be compatible with Arduino IDE.
  constexpr ServerDescription(const Literal& server_name,
                              const Literal& manufacturer,
                              const Literal& manufacturer_version,
                              const Literal& location)
      : server_name(server_name),
        manufacturer(manufacturer),
        manufacturer_version(manufacturer_version),
        location(location) {}

  // Write the description of this server to the specified JsonObjectEncoder.
  // The encoder should be for the nested object that is the value of the
  // "Value" property of the response object, NOT the outermost object that is
  // the body of the response to /man
  void AddTo(JsonObjectEncoder& object_encoder) const;

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

  const Literal server_name;
  const Literal manufacturer;
  const Literal manufacturer_version;
  const Literal location;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_DESCRIPTION_H_
