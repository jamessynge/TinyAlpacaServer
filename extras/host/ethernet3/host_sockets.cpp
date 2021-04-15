#include "extras/host/ethernet3/host_sockets.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/ioctls.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <memory>

#include "extras/host/ethernet3/w5500.h"
#include "logging.h"
#include "utils/logging.h"

namespace alpaca {

namespace {

bool set_non_blocking(int fd) {
  int fcntl_return = fcntl(fd, F_GETFL, 0);
  if (fcntl_return < 0) {
    auto msg = std::strerror(errno);
    LOG(ERROR) << "Failed to get file status flags for fd " << fd
               << ", error message: " << msg;
    return false;
  }
  if (fcntl_return & O_NONBLOCK) {
    return true;
  }
  fcntl_return = fcntl(fd, F_SETFL, fcntl_return | O_NONBLOCK);
  if (fcntl_return < 0) {
    auto msg = std::strerror(errno);
    LOG(ERROR) << "Failed to set file status flags for fd " << fd
               << ", error message: " << msg;
    return false;
  }
  return true;
}

struct HostSocketInfo {
  explicit HostSocketInfo(int socket_number) : sock_num(socket_number) {
    VLOG(1) << "Create HostSocketInfo for socket " << sock_num;
  }

  bool DisconnectConnectionSocket() {
    if (connection_socket >= 0) {
      VLOG(1) << "Disconnecting connection (" << connection_socket
              << ") for socket " << sock_num;
      if (::shutdown(connection_socket, SHUT_WR) == 0) {
        return true;
      }
    }
    return false;
  }

  void CloseConnectionSocket() {
    if (connection_socket >= 0) {
      VLOG(1) << "Closing connection (" << connection_socket << ") for socket "
              << sock_num;
      ::close(connection_socket);
    }
    connection_socket = -1;
  }

  void CloseListenerSocket() {
    if (listener_socket >= 0) {
      VLOG(1) << "Closing listener (" << connection_socket << ") for socket "
              << sock_num;
      ::close(listener_socket);
    }
    listener_socket = -1;
    tcp_port = 0;
  }

  // Start (or continue) listening for new TCP connections on 'tcp_port';
  // if currently connected to a peer, disconnect.
  bool InitializeTcpListener(uint16_t new_tcp_port) {
    VLOG(1) << "InitializeTcpListener on port " << new_tcp_port
            << " for socket " << sock_num;
    CloseConnectionSocket();
    if (listener_socket >= 0) {
      if (tcp_port == new_tcp_port) {
        return true;
      }
      CloseListenerSocket();
    }
    listener_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener_socket < 0) {
      LOG(ERROR) << "Unable to create listener for socket " << sock_num;
      return false;
    }
    int value = 1;
    socklen_t len = sizeof(value);
    if (::setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                     &value, len) < 0) {
      LOG(ERROR) << "Unable to set REUSEADDR for socket " << sock_num;
      CloseListenerSocket();
      return false;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(new_tcp_port);
    if (::bind(listener_socket, reinterpret_cast<sockaddr*>(&addr),
               sizeof addr) < 0) {
      auto error_number = errno;
      LOG(ERROR) << "Unable to set bind socket " << sock_num
                 << " to INADDR_ANY:" << new_tcp_port
                 << ", error message: " << std::strerror(error_number);
      CloseListenerSocket();
      return false;
    }
    if (!set_non_blocking(listener_socket)) {
      LOG(ERROR) << "Unable to make listener non-blocking for socket "
                 << sock_num;
      return false;
    }
    if (::listen(listener_socket, 9) < 0) {
      CloseListenerSocket();
      return false;
    }
    tcp_port = new_tcp_port;
    VLOG(1) << "Socket " << sock_num << " (fd " << listener_socket
            << ") is now listening for connections to port " << tcp_port;
    return true;
  }

  bool AcceptConnection() {
    DCHECK_LT(connection_socket, 0);
    VLOG(2) << "AcceptConnection for socket " << sock_num;
    if (listener_socket >= 0) {
      sockaddr_in addr;
      socklen_t addrlen = sizeof addr;
      connection_socket = ::accept(
          listener_socket, reinterpret_cast<sockaddr*>(&addr), &addrlen);
      if (connection_socket >= 0) {
        VLOG(1) << "Accepted a connection for socket " << sock_num
                << " with fd " << connection_socket;
        if (!set_non_blocking(listener_socket)) {
          LOG(WARNING) << "Unable to make connection non-blocking for socket "
                       << sock_num;
        }
        // The W5500 doesn't keep track of that fact that the socket used to be
        // listening, so to be a better emulation of its behavior, we now close
        // the listener socket, and will re-open it later if requested.
        CloseListenerSocket();
        return true;
      }
      int error_number = errno;
      VLOG(3) << "accept for socket " << sock_num
              << " failed with error message: " << std::strerror(error_number);
    }
    return false;
  }

  bool CanReadFromConnection() {
    if (connection_socket < 0) {
      LOG(ERROR) << "Socket " << sock_num << " isn't open.";
      return false;
    } else {
      while (true) {
        // See if we can peek at the next byte.
        char c;
        const auto ret =
            recv(connection_socket, &c, 1, MSG_PEEK | MSG_DONTWAIT);
        const auto error_number = errno;
        DVLOG(1) << "recv -> " << ret;
        if (ret > 0) {
          CHECK_EQ(ret, 1);
          // There is data available for reading.
          return true;
        } else if (ret < 0) {
          DVLOG(1) << "errno " << error_number << ": "
                   << std::strerror(error_number);
          if (error_number == EINTR) {
            // We were interrupted, so try again.
            continue;

          } else if (error_number == EAGAIN || error_number == EWOULDBLOCK) {
            // Reading would block, so peer must NOT have called shutdown with
            // how==SHUT_WR.
            return true;
          } else {
            // Some other error (invalid socket, etc.). Assume that this means
            // we can't read from the connection.
            return false;
          }
        } else {
          // ret == 0: for a TCP socket, this means that the peer has shutdown
          // the socket, at least for writing.
          return false;
        }
      }
    }
  }

  bool IsConnectionHalfClosed() {
    if (connection_socket < 0) {
      LOG(ERROR) << "Socket " << sock_num
                 << " isn't even open, can't be half closed.";
      return false;
    } else {
      // See if we can peek at the next byte.
      char c;
      return 0 == recv(connection_socket, &c, 1, MSG_PEEK);
    }
  }

  bool IsConnected() { return connection_socket >= 0; }

  bool IsClosed() { return listener_socket < 0 && connection_socket < 0; }

  bool IsUnused() {
    return listener_socket < 0 && connection_socket < 0 && tcp_port == 0;
  }

  int AvailableBytes() {
    if (connection_socket >= 0) {
      int bytes_available;
      if (ioctl(connection_socket, FIONREAD, &bytes_available) == 0) {
        return bytes_available;
      } else {
        const auto error_number = errno;
        LOG(WARNING) << "AvailableBytes: got errno " << error_number
                     << " reading FIONREAD, " << std::strerror(error_number);
      }
    }
    return 0;
  }

  const int sock_num;
  int listener_socket = -1;
  int connection_socket = -1;
  uint16_t tcp_port = 0;
};

HostSocketInfo* GetHostSocketInfo(int sock_num) {
  if (!(0 <= sock_num && sock_num <= MAX_SOCK_NUM)) {
    LOG(ERROR) << "Invalid socket number: " << sock_num;
    return nullptr;
  }
  using SocketInfoMap = std::map<int, std::unique_ptr<HostSocketInfo>>;
  static SocketInfoMap* host_sockets = nullptr;
  if (host_sockets == nullptr) {
    host_sockets = new SocketInfoMap();
    for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
      (*host_sockets)[sock_num] = std::make_unique<HostSocketInfo>(sock_num);
    }
  }
  return (*host_sockets)[sock_num].get();
}

}  // namespace

bool HostSockets::InitializeTcpListenerSocket(int sock_num, uint16_t tcp_port) {
  if (0 == tcp_port) {
    LOG(ERROR) << "tcp_port must not be zero";
    return false;
  }
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->InitializeTcpListener(tcp_port);
  }
  return false;
}

int HostSockets::InitializeTcpListenerSocket(uint16_t tcp_port) {
  for (int sock_num = 0; sock_num < MAX_SOCK_NUM; ++sock_num) {
    auto* info = GetHostSocketInfo(sock_num);
    if (info != nullptr && info->IsUnused()) {
      if (info->InitializeTcpListener(tcp_port)) {
        return sock_num;
      }
    }
  }
  return -1;
}

bool HostSockets::AcceptConnection(int sock_num) {
  VLOG(1) << "AcceptConnection(" << sock_num << ")";
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    if (info->connection_socket < 0) {
      return info->AcceptConnection();
    }
  }
  return false;
}

bool HostSockets::IsConnected(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->IsConnected();
  }
  return false;
}

bool HostSockets::Disconnect(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->DisconnectConnectionSocket();
  }
  return false;
}

bool HostSockets::IsClientDone(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->IsConnectionHalfClosed();
  }
  return false;
}

bool HostSockets::IsOpenForWriting(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->IsConnectionHalfClosed();
  }
  return false;
}

bool HostSockets::SocketIsClosed(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->IsClosed();
  }
  return false;
}

uint8_t HostSockets::SocketStatus(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr && !info->IsUnused()) {
    if (info->connection_socket >= 0) {
      if (info->CanReadFromConnection()) {
        VLOG(1) << "CanReadFromConnection -> true";
        return SnSR::ESTABLISHED;
      } else if (info->IsConnectionHalfClosed()) {
        VLOG(1) << "IsConnectionHalfClosed -> true";
        return SnSR::CLOSE_WAIT;
      }
    } else if (info->listener_socket >= 0) {
      if (info->AcceptConnection()) {
        return SnSR::ESTABLISHED;
      } else {
        return SnSR::LISTEN;
      }
    } else {
      CHECK(false) << "Why are we here?";
    }
  }
  return SnSR::CLOSED;
}

int HostSockets::AvailableBytes(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->AvailableBytes();
  } else {
    return 0;
  }
}

}  // namespace alpaca
