#ifndef __LOEVENT_SOCKET__
#define __LOEVENT_SOCKET__

#include "spdlog/spdlog.h"
#include "utils.hpp"

namespace loevent {

class Socket : noncopyable {
 public:
  Socket(int fd) : sockfd_(fd) {}
  ~Socket() {}
  int send(const void *buf, int len) {
    spdlog::debug("[socketSend] | len: {} | fd: {}", len, sockfd_);
    // printHexDump((char *)buf, len);
    return ::send(sockfd_, buf, len, 0);
  }
  int nonBlockSend(const void *buf, int len) {
    spdlog::debug("[socketSend] | len: {} | fd: {}", len, sockfd_);
    // printHexDump((char *)buf, len);
    return ::send(sockfd_, buf, len, MSG_DONTWAIT);
  }
  int getFd() { return sockfd_; }

 private:
  int sockfd_;
  int state_;
};

}  // namespace loevent
#endif  // !__LOEVENT_SOCKET__
