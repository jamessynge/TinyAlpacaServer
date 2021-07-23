#ifndef TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_IO_CONNECTION_H_
#define TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_IO_CONNECTION_H_

// Implements alpaca::Connection for testing, reading from a string_view,
// writing to a string.

#include <stddef.h>
#include <stdint.h>

#include <cstring>
#include <string>
#include <string_view>

#include "utils/connection.h"

namespace alpaca {
namespace test {

class StringIoConnection : public Connection {
 public:
  StringIoConnection(uint8_t sock_num, std::string_view input, bool half_closed)
      : input_(input),
        is_open_(true),
        half_closed_(half_closed),
        sock_num_(sock_num) {}

  int available() override { return is_open_ ? input_.size() : -1; }

  int peek() override {
    if (!is_open_) {
      return -1;
    } else if (input_.empty()) {
      return -1;
    } else {
      return input_[0];
    }
  }

  int read() override {
    if (!is_open_) {
      return -1;
    } else if (input_.empty()) {
      return -1;
    }
    char c = input_[0];
    input_.remove_prefix(1);
    return c;
  }

  size_t read(uint8_t* buf, size_t size) override {
    if (!is_open_) {
      return -1;
    }
    size = std::min(size, input_.size());
    if (size > 0) {
      std::memcpy(buf, input_.data(), size);
      input_.remove_prefix(size);
    }
    return size;
  }

  using Connection::read;

  size_t write(uint8_t b) override {
    if (!is_open_) {
      return -1;
    }
    output_.push_back(static_cast<char>(b));
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    if (!is_open_) {
      return -1;
    }
    output_.append(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  using Connection::write;

  void close() override { is_open_ = false; }

  bool connected() const override { return is_open_; }

  bool peer_half_closed() const override {
    return is_open_ && input_.empty() && half_closed_;
  }

  uint8_t sock_num() const override { return sock_num_; }

  std::string_view remaining_input() const { return input_; }

  const std::string& output() const { return output_; }

 private:
  std::string_view input_;
  std::string output_;
  bool is_open_;
  bool half_closed_;
  const uint8_t sock_num_;
};

}  // namespace test
}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_EXTRAS_TEST_TOOLS_STRING_IO_CONNECTION_H_
