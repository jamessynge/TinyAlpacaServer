#ifndef TINY_ALPACA_SERVER_SRC_SERVER_H_
#define TINY_ALPACA_SERVER_SRC_SERVER_H_

#include "src/platform.h"

namespace alpaca {

class Server {
 public:
  static uint32_t GetNextServerTransactionId() {
    return ++server_transaction_id_;
  }

 private:
  static uint32_t server_transaction_id_;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_SERVER_H_
