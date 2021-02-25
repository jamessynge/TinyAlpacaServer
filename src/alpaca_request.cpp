#include "src/alpaca_request.h"

// Author: james.synge@gmail.com

#include "src/decoder_constants.h"

namespace alpaca {

void AlpacaRequest::Reset() {
  found_client_id = false;
  found_client_transaction_id = false;
  have_server_transaction_id = false;

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  extra_parameters.clear();
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS

#if TAS_ENABLE_DEBUGGING
  http_method = EHttpMethod::kUnknown;
  device_type = EDeviceType::kUnknown;
  device_number = kResetDeviceNumber;
  ascom_method = EMethod::kUnknown;
  client_id = kResetClientId;
  client_transaction_id = kResetClientTransactionId;
  server_transaction_id = kResetServerTransactionId;
#endif  // TAS_ENABLE_DEBUGGING
}

}  // namespace alpaca
