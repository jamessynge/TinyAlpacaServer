#ifndef TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_

#include "src/alpaca_request.h"
#include "src/json_encoder.h"
#include "src/platform.h"

namespace alpaca {

// Writes the common portion shared by all Alpaca responses.
class CommonJsonResponse : public JsonPropertySource {
 public:
  explicit CommonJsonResponse(const AlpacaRequest& request)
      : CommonJsonResponse(request, 0, nullptr) {}

  CommonJsonResponse(const AlpacaRequest& request, uint32_t error_number,
                     const char* error_message)
      : request_(request),
        error_number_(error_number),
        error_message_(error_message ? error_message : "") {}

  void AddTo(JsonObjectEncoder& object_encoder) override {
    if (request_.found_client_transaction_id) {
      object_encoder.AddIntegerProperty("ClientTransactionId",
                                        request_.client_transaction_id);
    }
    if (request_.have_server_transaction_id) {
      object_encoder.AddIntegerProperty("ServerTransactionId",
                                        request_.server_transaction_id);
    }
    object_encoder.AddIntegerProperty("ErrorNumber", error_number_);
    object_encoder.AddStringProperty("ErrorMessage",
                                     StringView::FromCString(error_message_));
  }

 private:
  const AlpacaRequest& request_;
  const uint32_t error_number_;
  const char* const error_message_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_
