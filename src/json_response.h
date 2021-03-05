#ifndef TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_

// Implementations of JsonPropertySource for the Alpaca API schemas needed for
// the device types that I intend to support in the short term.
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "any_string.h"
#include "json_encoder.h"
#include "literals.h"
#include "platform.h"
#include "string_view.h"

namespace alpaca {

// Writes the common portion shared by all Alpaca responses.
class JsonMethodResponse : public JsonPropertySource {
 public:
  explicit JsonMethodResponse(const AlpacaRequest& request)
      : JsonMethodResponse(request, 0, StringView()) {}

  // If the error_message is a StringView, the underlying string must remain
  // valid while the response object exists.
  JsonMethodResponse(const AlpacaRequest& request, uint32_t error_number,
                     const AnyString& error_message)
      : request_(request),
        error_number_(error_number),
        error_message_(error_message) {}

  void AddTo(JsonObjectEncoder& object_encoder) const override {
    if (request_.have_client_transaction_id) {
      object_encoder.AddIntegerProperty(Literals::ClientTransactionId(),
                                        request_.client_transaction_id);
    }
    if (request_.have_server_transaction_id) {
      object_encoder.AddIntegerProperty(Literals::ServerTransactionId(),
                                        request_.server_transaction_id);
    }
    object_encoder.AddIntegerProperty(Literals::ErrorNumber(), error_number_);
    object_encoder.AddStringProperty(Literals::ErrorMessage(), error_message_);
  }

 private:
  const AlpacaRequest& request_;
  const uint32_t error_number_;
  const AnyString error_message_;
};

class JsonArrayResponse : public JsonMethodResponse {
 public:
  JsonArrayResponse(const AlpacaRequest& request,
                    const JsonElementSource& value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddArrayProperty(Literals::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const JsonElementSource& value_;
};

class JsonBoolResponse : public JsonMethodResponse {
 public:
  JsonBoolResponse(const AlpacaRequest& request, bool value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddBooleanProperty(Literals::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const bool value_;
};

class JsonDoubleResponse : public JsonMethodResponse {
 public:
  JsonDoubleResponse(const AlpacaRequest& request, double value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddFloatingPointProperty(Literals::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const double value_;
};

class JsonFloatResponse : public JsonMethodResponse {
 public:
  JsonFloatResponse(const AlpacaRequest& request, float value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddFloatingPointProperty(Literals::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const float value_;
};

class JsonStringResponse : public JsonMethodResponse {
 public:
  JsonStringResponse(const AlpacaRequest& request, AnyString value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddStringProperty(Literals::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const AnyString value_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_
