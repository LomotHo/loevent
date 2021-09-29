#ifndef __LOMOT_REACTOR_UTILS__
#define __LOMOT_REACTOR_UTILS__

#include <sys/socket.h>

#include <iostream>
#include <memory>
#include <string>

class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  void operator=(const noncopyable &) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

class Socket : noncopyable {
 public:
  Socket(int fd) : sockfd(fd) {}
  // ~Socket() {}
  int sockfd;
  void socketSend(const void *message, int len) {
    send(sockfd, message, len, 0);
  }

 private:
  int state_;
};

void error_quit(std::string msg) {
  std::cout << msg << std::endl;
  exit(1);
}

#endif  // !__LOMOT_REACTOR_UTILS__
