// TODO(jamessynge): Describe why this file exists/what it provides.

#include "server_description.h"

#include "literals.h"
#include "utils/json_encoder.h"

namespace alpaca {

void ServerDescription::AddTo(JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(Literals::ServerName(), server_name);
  object_encoder.AddStringProperty(Literals::Manufacturer(), manufacturer);
  object_encoder.AddStringProperty(Literals::ManufacturerVersion(),
                                   manufacturer_version);
  object_encoder.AddStringProperty(Literals::Location(), location);
}

}  // namespace alpaca
