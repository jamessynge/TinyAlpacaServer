#include "alpaca_request.h"

// Author: james.synge@gmail.com

#include "decoder_constants.h"

namespace alpaca {

void AlpacaRequest::Reset() {
  http_method = EHttpMethod::kUnknown;

  found_client_id = false;
  found_client_transaction_id = false;
  have_server_transaction_id = false;

  // Theoretically we don't need to clear the following fields because they
  // shouldn't be examined unless the decoder has returned
  // EDecodeStatus::kHttpOk. However, it makes writing tests easier if we do so.
  api_group = EApiGroup::kUnknown;
  api = EAlpacaApi::kUnknown;
  device_type = EDeviceType::kUnknown;
  device_number = kResetDeviceNumber;
  device_method = EDeviceMethod::kUnknown;

  // This is a value set at the start of the decoding process.
  server_transaction_id = kResetServerTransactionId;

  // Parameters that may be provided for all requests, so we handle them
  // specifically.
  client_id = kResetClientId;
  client_transaction_id = kResetClientTransactionId;

#if TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
  extra_parameters.clear();
#endif  // TAS_ENABLE_EXTRA_REQUEST_PARAMETERS
}

}  // namespace alpaca
