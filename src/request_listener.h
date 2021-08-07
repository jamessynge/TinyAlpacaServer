#ifndef TINY_ALPACA_SERVER_SRC_REQUEST_LISTENER_H_
#define TINY_ALPACA_SERVER_SRC_REQUEST_LISTENER_H_

// API used by ServerConnection to deliver decoded requests to the server.
//
// Author: james.synge@gmail.com

#include "alpaca_request.h"
#include "constants.h"
#include "mcucore_platform.h"

namespace alpaca {

class RequestListener {
 public:
  virtual ~RequestListener() {}

  // Called when the first byte of a request is being decoded (which might be
  // whitespace between requests). This allows the callee to set
  // request.server_transaction_id.
  virtual void OnStartDecoding(AlpacaRequest& request) = 0;

  // Called when a request has been successfully decoded. 'out' should be used
  // to write a response to the client. Return true to continue decoding more
  // requests from the client, false to disconnect.
  virtual bool OnRequestDecoded(AlpacaRequest& request, Print& out) = 0;

  // Called when decoding of a request has failed. 'out' should be used to write
  // an error response to the client. The connection to the client will be
  // closed after the response is returned.
  virtual void OnRequestDecodingError(AlpacaRequest& request,
                                      EHttpStatusCode status, Print& out) = 0;
};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_REQUEST_LISTENER_H_
