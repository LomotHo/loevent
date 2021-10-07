#ifndef __LOMOT_REACTOR_CONNECTOR__
#define __LOMOT_REACTOR_CONNECTOR__

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstring>

#include "utils.hpp"

namespace loevent {

class Connector {
 public:
  ~Connector() {}
  Connector(const char *ip, int port) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
      perror("socket");
      return;
    }
    bzero(&servaddr_, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_port = htons(port);
    inet_pton(AF_INET, ip, &servaddr_.sin_addr);
  }
  int doConnect() {
    int n;
    n = connect(sockfd_, (struct sockaddr *)&servaddr_, sizeof(servaddr_));
    if (n < 0) {
      perror("connect");
    }
    return n;
  }
  int getFd() { return sockfd_; }

 private:
  struct sockaddr_in servaddr_;
  int sockfd_;
};

}  // namespace loevent
#endif  // !__LOMOT_REACTOR_CONNECTOR__
