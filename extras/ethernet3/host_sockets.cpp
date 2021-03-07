#include "extras/ethernet3/host_sockets.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>

#include "extras/ethernet3/ethernet_config.h"
#include "logging.h"

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
    VLOG(1) << "Socket " << sock_num
            << " is now listening for connections to port " << tcp_port;
    return true;
  }

  bool AcceptConnection() {
    DCHECK_LT(connection_socket, 0);
    VLOG(1) << "AcceptConnection for socket " << sock_num;
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
        return true;
      }
      int error_number = errno;
      VLOG(1) << "accept for socket " << sock_num
              << " failed with error message: " << std::strerror(error_number);
    }
    return false;
  }

  bool IsConnectionHalfClosed() {
    if (connection_socket < 0) {
      LOG(ERROR) << "Socket " << sock_num
                 << " isn't even open, can't be half closed.";
      return false;
    } else {
      // See if we can peek at the
      char c;
      return 0 == recv(connection_socket, &c, 1, MSG_PEEK);
    }
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

bool InitializeTcpListenerSocket(int sock_num, uint16_t tcp_port) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    if (tcp_port == 0) {
      LOG(ERROR) << "tcp_port must not be zero";
    } else {
      return info->InitializeTcpListener(tcp_port);
    }
  }
  return false;
}

bool AcceptConnection(int sock_num) {
  VLOG(1) << "AcceptConnection(" << sock_num << ")";
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    if (info->connection_socket < 0) {
      return info->AcceptConnection();
    }
  }
  return false;
}

bool IsClientDone(int sock_num) {
  auto* info = GetHostSocketInfo(sock_num);
  if (info != nullptr) {
    return info->IsConnectionHalfClosed();
  }
  return false;
}

}  // namespace alpaca
