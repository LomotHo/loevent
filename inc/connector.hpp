#ifndef __LOEVENT_CONNECTOR__
#define __LOEVENT_CONNECTOR__

#include <arpa/inet.h>
// #include <fcntl.h>
#include <netinet/in.h>
#include <spdlog/spdlog.h>

#include <cstring>

#include "utils.hpp"

namespace loevent {

class Connector {
 public:
  ~Connector() {}
  Connector(const char *ip, int port) {
    sockfd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd_ < 0) {
      spdlog::error("[socket] {}: {} | sockfd: {}", errno, strerror(errno), sockfd_);
      return;
    }
    // fcntl(sockfd_, F_SETFL, O_NONBLOCK);
    bzero(&servaddr_, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_port = htons(port);
    inet_pton(AF_INET, ip, &servaddr_.sin_addr);
  }

  int doConnect() {
    return connect(sockfd_, (struct sockaddr *)&servaddr_, sizeof(servaddr_));
  }
  int getFd() { return sockfd_; }

 private:
  struct sockaddr_in servaddr_;
  int sockfd_;
};

}  // namespace loevent
#endif  // !__LOEVENT_CONNECTOR__
