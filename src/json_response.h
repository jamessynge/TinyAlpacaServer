#ifndef TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_
#define TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_

// Implementations of mcucore::JsonPropertySource for the Alpaca API schemas
// needed for the device types that I intend to support in the short term.
//
// Author: james.synge@gmail.com

#include <McuCore.h>

#include "alpaca_request.h"
#include "literals.h"

namespace alpaca {

// Writes the common portion shared by all Alpaca responses.
class JsonMethodResponse : public mcucore::JsonPropertySource {
 public:
  explicit JsonMethodResponse(const AlpacaRequest& request)
      : request_(request), error_number_(0), error_message_(nullptr) {}

  JsonMethodResponse(const AlpacaRequest& request, uint32_t error_number,
                     const Printable& error_message)
      : request_(request),
        error_number_(error_number),
        error_message_(&error_message) {}

  ~JsonMethodResponse() override {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    if (request_.have_client_transaction_id) {
      object_encoder.AddUIntProperty(ProgmemStringViews::ClientTransactionID(),
                                     request_.client_transaction_id);
    }
    if (request_.have_server_transaction_id) {
      object_encoder.AddUIntProperty(ProgmemStringViews::ServerTransactionID(),
                                     request_.server_transaction_id);
    }
    if (error_number_ != 0) {
      object_encoder.AddUIntProperty(ProgmemStringViews::ErrorNumber(),
                                     error_number_);
    }
    if (error_message_ != nullptr) {
      object_encoder.AddStringProperty(ProgmemStringViews::ErrorMessage(),
                                       *error_message_);
    }
  }

 private:
  // Make JsonMethodResponse non-copyable; also makes it non-moveable.
  JsonMethodResponse(const JsonMethodResponse&) = delete;
  JsonMethodResponse& operator=(const JsonMethodResponse&) = delete;

  const AlpacaRequest& request_;
  const uint32_t error_number_;
  const Printable* error_message_;
};

class JsonArrayResponse : public JsonMethodResponse {
 public:
  JsonArrayResponse(const AlpacaRequest& request,
                    const mcucore::JsonElementSource& value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddArrayProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const mcucore::JsonElementSource& value_;
};

class JsonBoolResponse : public JsonMethodResponse {
 public:
  JsonBoolResponse(const AlpacaRequest& request, bool value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddBooleanProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const bool value_;
};

class JsonDoubleResponse : public JsonMethodResponse {
 public:
  JsonDoubleResponse(const AlpacaRequest& request, double value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddDoubleProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const double value_;
};

class JsonFloatResponse : public JsonMethodResponse {
 public:
  JsonFloatResponse(const AlpacaRequest& request, float value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddFloatProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const float value_;
};

class JsonUnsignedIntegerResponse : public JsonMethodResponse {
 public:
  JsonUnsignedIntegerResponse(const AlpacaRequest& request, uint32_t value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddUIntProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const uint32_t value_;
};

class JsonIntegerResponse : public JsonMethodResponse {
 public:
  JsonIntegerResponse(const AlpacaRequest& request, int32_t value)
      : JsonMethodResponse(request), value_(value) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddIntProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const int32_t value_;
};

class JsonStringResponse : public JsonMethodResponse {
 public:
  JsonStringResponse(const AlpacaRequest& request, mcucore::AnyPrintable value)
      : JsonMethodResponse(request), value_(value) {}
  JsonStringResponse(const AlpacaRequest& request,
                     mcucore::ProgmemStringView value)
      : JsonMethodResponse(request), value_(mcucore::AnyPrintable(value)) {}
  JsonStringResponse(const AlpacaRequest& request, const Printable& value)
      : JsonMethodResponse(request), value_(mcucore::AnyPrintable(value)) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddStringProperty(ProgmemStringViews::Value(), value_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  mcucore::AnyPrintable value_;
};

class JsonObjectResponse : public JsonMethodResponse {
 public:
  JsonObjectResponse(const AlpacaRequest& request,
                     const mcucore::JsonPropertySource& property_source)
      : JsonMethodResponse(request), property_source_(property_source) {}

  void AddTo(mcucore::JsonObjectEncoder& object_encoder) const override {
    object_encoder.AddObjectProperty(ProgmemStringViews::Value(),
                                     property_source_);
    JsonMethodResponse::AddTo(object_encoder);
  }

 private:
  const mcucore::JsonPropertySource& property_source_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_JSON_RESPONSE_H_
