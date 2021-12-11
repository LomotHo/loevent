#ifndef __LOEVENT_ACCEPTER__
#define __LOEVENT_ACCEPTER__

// #include <fcntl.h>
#include <netinet/in.h>

#include <cstring>

#include "utils.hpp"

namespace loevent {

class Accepter {
 public:
  ~Accepter() {}
  Accepter(int port) {
    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_.sin_port = htons(port);

    if ((listenfd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
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
  int getFd() { return listenfd_; }
  inline int doAccept() {
    // int sockfd =
    //     accept4(listenfd_, (struct sockaddr *)&cliaddr_, &clilen_, SOCK_NONBLOCK);
    // // accept(listenfd_, (struct sockaddr *)&cliaddr, &clilen);
    // if (sockfd == -1) {
    //   return -1;
    // }
    // return sockfd;
    return accept4(listenfd_, (struct sockaddr *)&cliaddr_, &clilen_, SOCK_NONBLOCK);
  }

 private:
  int listenfd_;
  struct sockaddr_in servaddr_;
  struct sockaddr_in cliaddr_;
  socklen_t clilen_ = sizeof(cliaddr_);
};

}  // namespace loevent
#endif  // !__LOEVENT_ACCEPTER__
