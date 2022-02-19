#include "server_connection.h"

#include <McuCore.h>

#include "alpaca_response.h"
#include "constants.h"
#include "experimental/users/jamessynge/arduino/mcunet/src/platform_ethernet.h"
#include "literals.h"
#include "request_listener.h"

#if MCU_HOST_TARGET
#include <string.h>
#endif

namespace alpaca {

ServerConnection::ServerConnection(RequestListener& request_listener)
    : request_listener_(request_listener),
      request_decoder_(request_),
      sock_num_(MAX_SOCK_NUM) {
  MCU_VLOG(4) << MCU_FLASHSTR("ServerConnection @ ") << this
              << MCU_FLASHSTR(" ctor");
}

void ServerConnection::OnConnect(mcunet::Connection& connection) {
  MCU_VLOG(2) << MCU_FLASHSTR("ServerConnection @ ") << this
              << MCU_FLASHSTR(" ->::OnConnect ") << connection.sock_num();
  MCU_DCHECK(!has_socket());
  sock_num_ = connection.sock_num();
  request_decoder_.Reset();
  between_requests_ = true;
  input_buffer_size_ = 0;
}

void ServerConnection::OnCanRead(mcunet::Connection& connection) {
  MCU_VLOG(5) << MCU_FLASHSTR("ServerConnection @ ") << this
              << MCU_FLASHSTR(" ->::OnCanRead ") << MCU_FLASHSTR("socket ")
              << connection.sock_num();
  MCU_DCHECK_EQ(sock_num(), connection.sock_num());
  MCU_DCHECK(request_decoder_.status() == RequestDecoderStatus::kReset ||
             request_decoder_.status() == RequestDecoderStatus::kDecoding);
  // Load input_buffer_ with as much data as will fit.
  if (input_buffer_size_ < sizeof input_buffer_) {
    auto ret = connection.read(
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

    mcucore::StringView view(input_buffer_, input_buffer_size_);
    const bool buffer_is_full = input_buffer_size_ == sizeof input_buffer_;
    const bool at_end = connection.peer_half_closed();

    EHttpStatusCode status_code =
        request_decoder_.DecodeBuffer(view, buffer_is_full, at_end);

    // Update the input buffer to reflect that some input has (hopefully) been
    // decoded.
    if (view.empty()) {
      input_buffer_size_ = 0;
    } else {
      // Verify that any removed bytes constitute a prefix of input_buffer_.
      MCU_DCHECK_LE(view.size(), input_buffer_size_);
      MCU_DCHECK_EQ(view.data() + view.size(),
                    input_buffer_ + input_buffer_size_);

      if (view.size() < input_buffer_size_) {
        // Move the undecoded bytes to the front of the buffer.
        input_buffer_size_ = view.size();
        memmove(input_buffer_, view.data(), view.size());
      }
    }

    // Are we done decoding?
    if (status_code < EHttpStatusCode::kHttpOk) {
      // No.
      return;
    }

    bool close_connection = false;
    if (status_code == EHttpStatusCode::kHttpOk) {
      MCU_VLOG(4) << MCU_FLASHSTR("ServerConnection @ ") << this
                  << MCU_FLASHSTR(" ->::OnCanRead ")
                  << MCU_FLASHSTR("status_code: ") << status_code;
      if (input_buffer_size_ == 0) {
        between_requests_ = true;
      }
      if (!request_listener_.OnRequestDecoded(request_, connection)) {
        close_connection = true;
      }
    } else {
      MCU_VLOG(3) << MCU_FLASHSTR("ServerConnection @ ") << this
                  << MCU_FLASHSTR(" ->::OnCanRead ")
                  << MCU_FLASHSTR("status_code: ") << status_code;
      request_listener_.OnRequestDecodingError(request_, status_code,
                                               connection);
      close_connection = true;
    }

    // If we've returned an error, then we also close the connection so that
    // we don't require finding the end of a corrupt input request.
    if (close_connection) {
      MCU_VLOG(3) << MCU_FLASHSTR("ServerConnection @ ") << this
                  << MCU_FLASHSTR(" ->::OnCanRead ")
                  << MCU_FLASHSTR("closing connection");

      connection.close();
      sock_num_ = MAX_SOCK_NUM;
    } else {
      // Prepare the decoder for the next request.
      request_decoder_.Reset();
    }
  }
}

void ServerConnection::OnHalfClosed(mcunet::Connection& connection) {
  MCU_DCHECK_EQ(sock_num(), connection.sock_num());

  if (!between_requests_) {
    // We've read some data but haven't been able to decode a complete request.
    MCU_VLOG(3) << MCU_FLASHSTR("ServerConnection @ ") << this
                << MCU_FLASHSTR(" ->::OnHalfClosed socket ")
                << connection.sock_num() << MCU_FLASHSTR(" between_requests_=")
                << between_requests_;
    request_listener_.OnRequestDecodingError(
        request_, EHttpStatusCode::kHttpBadRequest, connection);
  } else {
    MCU_VLOG(4) << MCU_FLASHSTR("ServerConnection @ ") << this
                << MCU_FLASHSTR(" ->::OnHalfClosed socket ")
                << connection.sock_num();
  }
  connection.close();
  sock_num_ = MAX_SOCK_NUM;
}

void ServerConnection::OnDisconnect() {
  MCU_VLOG(2) << MCU_FLASHSTR("ServerConnection @ ") << this
              << MCU_FLASHSTR(" ->::OnDisconnect, sock_num_=") << sock_num_;
  MCU_DCHECK(has_socket());
  sock_num_ = MAX_SOCK_NUM;
}

}  // namespace alpaca
