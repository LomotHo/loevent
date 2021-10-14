#ifndef __LOEVENT_SOCKET__
#define __LOEVENT_SOCKET__

#include "spdlog/spdlog.h"
#include "utils.hpp"

namespace loevent {

class Socket : noncopyable {
 public:
  Socket(int fd) : sockfd_(fd) {}
  ~Socket() {}
  void socketSend(const void *buf, int len) {
    spdlog::debug("[socketSend] | len: {} | fd: {}", len, sockfd_);
    // printhexDump((char *)buf, len);
    send(sockfd_, buf, len, 0);
  }
  int getFd() { return sockfd_; }

 private:
  int sockfd_;
  int state_;
};

}  // namespace loevent
#endif  // !__LOEVENT_SOCKET__
