#ifndef __LOMOT_REACTOR_SOCKET__
#define __LOMOT_REACTOR_SOCKET__

#include "spdlog/spdlog.h"
#include "utils.hpp"

class Socket : noncopyable {
 public:
  Socket(int fd) : sockfd_(fd) {}
  ~Socket() {}
  void socketSend(void *buf, int len) {
    // spdlog::debug("[socketSend] | len: {} | fd: {}", len, sockfd_);
    // printhexDump((char *)buf, len);
    send(sockfd_, buf, len, 0);
  }
  int getFd() { return sockfd_; }

 private:
  int sockfd_;
  int state_;
};

#endif  // !__LOMOT_REACTOR_SOCKET__
