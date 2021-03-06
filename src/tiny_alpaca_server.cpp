// TODO(jamessynge): Describe why this file exists/what it provides.

#include "tiny_alpaca_server.h"

#include "utils/platform.h"

namespace alpaca {

uint32_t TinyAlpacaServer::GetNextServerTransactionId() {
  return ++server_transaction_id_;
}

uint32_t TinyAlpacaServer::server_transaction_id_ = 0;

}  // namespace alpaca
