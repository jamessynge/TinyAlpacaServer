// TODO(jamessynge): Describe why this file exists/what it provides.

#include "tiny_alpaca_server.h"

#include "utils/platform.h"

namespace alpaca {

TinyAlpacaServer::TinyAlpacaServer(uint16_t tcp_port,
                                   const ServerDescription& server_description,
                                   DeviceApiHandlerBase* device_handlers,
                                   size_t num_device_handlers)
    : server_transaction_id_(0),
      server_description_(server_description),
      device_handlers_(device_handlers),
      num_device_handlers_(num_device_handlers) {
  InitializeServerConnections(tcp_port);
}

ServerConnection* TinyAlpacaServer::GetServerConnection(size_t ndx) {
  return reinterpret_cast<ServerConnection*>(connections_storage_) + ndx;
}

void TinyAlpacaServer::InitializeServerConnections(uint16_t tcp_port) {
  for (int ndx = 0; ndx < kNumServerConnections; ++ndx) {
    // Use 'placement new' to initialize an array within this TinyAlpacaServer
    // with references to this instance.
    void* memory = GetServerConnection(ndx);
    new (memory) ServerConnection(/*sock_num=*/ndx, tcp_port, *this);
  }
}

bool TinyAlpacaServer::begin() {
  for (int ndx = 0; ndx < kNumServerConnections; ++ndx) {
    auto* conn = GetServerConnection(ndx);
    if (!conn->BeginListening()) {
      TAS_LOG(ERROR) << "Unable to initialize ServerConnection #" << ndx;
      return false;
    }
  }
  // TODO(jamessynge): Add UDP listener too.
  return true;
}

// Performs network IO as appropriate.
void TinyAlpacaServer::loop() {}

void TinyAlpacaServer::OnStartDecoding(AlpacaRequest& request) {
  request.set_server_transaction_id(++server_transaction_id_);
}

// Called when a request has been successfully decoded. 'out' should be used
// to write a response to the client. Return true to continue decoding more
// requests from the client, false to disconnect.
bool TinyAlpacaServer::OnRequestDecoded(AlpacaRequest& request, Print& out) {
  // TODO(jamessynge): Lookup the handler based on the request, dispatch to that
  // code.

  return false;
}

// Called when decoding of a request has failed. 'out' should be used to write
// an error response to the client. The connection to the client will be
// closed after the response is returned.
void TinyAlpacaServer::OnRequestDecodingError(AlpacaRequest& request,
                                              EHttpStatusCode status,
                                              Print& out) {}

}  // namespace alpaca
