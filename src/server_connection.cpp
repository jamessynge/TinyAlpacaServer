// TODO(jamessynge): Describe why this file exists/what it provides.

#include "server_connection.h"

#include "alpaca_response.h"
#include "constants.h"
#include "literals.h"
#include "request_listener.h"
#include "utils/platform_ethernet.h"
#include "utils/string_view.h"

#if TAS_HOST_TARGET
#include <string.h>
#endif

namespace alpaca {

ServerConnection::ServerConnection(RequestListener& request_listener)
    : request_listener_(request_listener), request_decoder_(request_) {
  TAS_VLOG(2) << TASLIT("ServerConnection@0x") << this << TASLIT(" ctor");
}

void ServerConnection::OnConnect(EthernetClient& client) {
  TAS_VLOG(2) << TASLIT("ServerConnection@0x") << this
              << TASLIT(" ->::OnConnect socket ") << client.getSocketNumber();
  TAS_DCHECK_EQ(sock_num(), client.getSocketNumber());
  request_decoder_.Reset();
  between_requests_ = true;
  input_buffer_size_ = 0;
}

void ServerConnection::OnCanRead(EthernetClient& client) {
  TAS_VLOG(2) << TASLIT("ServerConnection@0x") << this
              << " ->::OnCanRead socket " << client.getSocketNumber();
  TAS_DCHECK_EQ(sock_num(), client.getSocketNumber());
  TAS_DCHECK(request_decoder_.status() == RequestDecoderStatus::kReset ||
             request_decoder_.status() == RequestDecoderStatus::kDecoding);
  // Load input_buffer_ with as much data as will fit.
  if (input_buffer_size_ < sizeof input_buffer_) {
    auto ret = client.read(
        reinterpret_cast<uint8_t*>(&input_buffer_[input_buffer_size_]),
        (sizeof input_buffer_) - input_buffer_size_);
    if (ret > 0) {
      input_buffer_size_ += ret;
      between_requests_ = false;
    }
  }

  // If there is data to be decoded, do so.
  if (input_buffer_size_ > 0) {
    if (request_decoder_.status() == RequestDecoderStatus::kReset) {
      request_listener_.OnStartDecoding(request_);
    }

    StringView view(input_buffer_, input_buffer_size_);
    const bool buffer_is_full = input_buffer_size_ == sizeof input_buffer_;
    const bool at_end =
        PlatformEthernet::IsClientDone(client.getSocketNumber());

    EHttpStatusCode status_code =
        request_decoder_.DecodeBuffer(view, buffer_is_full, at_end);

    // Update the input buffer to reflect that some input has (hopefully) been
    // decoded.
    if (view.empty()) {
      input_buffer_size_ = 0;
    } else {
      // Verify that any removed bytes constitute a prefix of input_buffer_.
      TAS_DCHECK_LE(view.size(), input_buffer_size_);
      TAS_DCHECK_EQ(view.data() + view.size(),
                    input_buffer_ + input_buffer_size_);

      if (view.size() < input_buffer_size_) {
        // Move the undecoded bytes to the front of the buffer.
        input_buffer_size_ = view.size();
        memcpy(input_buffer_, view.data(), view.size());
      }
    }

    // Are we done decoding?
    if (status_code < EHttpStatusCode::kHttpOk) {
      // No.
      return;
    }

    TAS_VLOG(2) << TASLIT("ServerConnection@0x") << this
                << TASLIT(" ->::OnCanRead status_code: ") << status_code;

    bool close_connection = false;
    if (status_code == EHttpStatusCode::kHttpOk) {
      if (input_buffer_size_ == 0) {
        between_requests_ = true;
      }
      if (!request_listener_.OnRequestDecoded(request_, client)) {
        close_connection = true;
      }
    } else {
      request_listener_.OnRequestDecodingError(request_, status_code, client);
      close_connection = true;
    }

    // Prepare the decoder for the next request.
    request_decoder_.Reset();

    // If we've returned an error, then we also close the connection so that
    // we don't require finding the end of a corrupt input request.
    if (close_connection) {
      client.stop();
    }
  }
}

void ServerConnection::OnClientDone(EthernetClient& client) {
  TAS_VLOG(2) << TASLIT("ServerConnection@0x") << this
              << TASLIT(" ->::OnClientDone socket ")
              << client.getSocketNumber();

  if (!between_requests_) {
    // We've read some data but haven't been able to decode a complete request.
    request_listener_.OnRequestDecodingError(
        request_, EHttpStatusCode::kHttpBadRequest, client);
  }
  client.stop();
}

void ServerConnection::OnDisconnect() {}

}  // namespace alpaca
