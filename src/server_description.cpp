#include "server_description.h"

#include <McuCore.h>

#include "literals.h"

namespace alpaca {

void ServerDescription::AddTo(
    mcucore::JsonObjectEncoder& object_encoder) const {
  object_encoder.AddStringProperty(ProgmemStringViews::ServerName(),
                                   server_name);
  object_encoder.AddStringProperty(ProgmemStringViews::Manufacturer(),
                                   manufacturer);
  object_encoder.AddStringProperty(ProgmemStringViews::ManufacturerVersion(),
                                   manufacturer_version);
  object_encoder.AddStringProperty(ProgmemStringViews::Location(), location);
}

}  // namespace alpaca
