#include "tiny-alpaca-server/decoder/request.h"

#include "tiny-alpaca-server/decoder/constants.h"

namespace alpaca {

void AlpacaRequest::Reset() {
  found_client_id = false;
  found_client_transaction_id = false;

#ifndef NDEBUG
  http_method = EHttpMethod::kUnknown;
  device_type = EDeviceType::kUnknown;
  device_number = kResetDeviceNumber;
  ascom_method = EMethod::kUnknown;
  client_id = kResetClientId;
  client_transaction_id = kResetClientTransactionId;
#endif
}

}  // namespace alpaca
