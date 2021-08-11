#include "server_description.h"

#include "json_encoder.h"
#include "literals.h"

namespace alpaca {

void ServerDescription::AddTo(
    mcucore::JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(Literals::ServerName(), server_name);
  object_encoder.AddStringProperty(Literals::Manufacturer(), manufacturer);
  object_encoder.AddStringProperty(Literals::ManufacturerVersion(),
                                   manufacturer_version);
  object_encoder.AddStringProperty(Literals::Location(), location);
}

}  // namespace alpaca
