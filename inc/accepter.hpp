#ifndef __LOMOT_REACTOR_ACCEPTER__
#define __LOMOT_REACTOR_ACCEPTER__

#include <netinet/in.h>

#include <cstring>

#include "utils.hpp"

class Accepter {
 public:
  ~Accepter() {}
  Accepter(int port) {
    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_.sin_port = htons(port);

    if ((listenfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      error_quit("Error creating socket..");
    }
    // fcntl(listenfd_, F_SETFL, O_NONBLOCK);
    int on = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(listenfd_, (struct sockaddr *)&servaddr_, sizeof(servaddr_)) < 0) {
      error_quit("error on bind");
    }

    if (listen(listenfd_, 8) < 0) {
      error_quit("error on listen");
    }
  }
  int getListenFd() { return listenfd_; }
  int doAccept() {
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int sockfd =
        accept4(listenfd_, (struct sockaddr *)&cliaddr, &clilen, SOCK_NONBLOCK);
    if (sockfd == -1) {
      error_quit("Error accepting new connection..");
      return -1;
    }
    return sockfd;
  }

 private:
  int listenfd_;
  struct sockaddr_in servaddr_;
};

#endif  // !__LOMOT_REACTOR_ACCEPTER__
