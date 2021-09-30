#ifndef __LOMOT_REACTOR_UTILS__
#define __LOMOT_REACTOR_UTILS__

#include "spdlog/spdlog.h"
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
  void socketSend(std::vector<char> msg) {
    spdlog::debug("socketSend {}", msg.data());
    // char *temp_ptr = msg.empty() ? 0 : &msg[0];
    char *str = new char[msg.size() + 1];
    copy(msg.begin(), msg.end(), str);
    str[msg.size()] = 0;

    send(sockfd, str, msg.size() + 1, 0);
  }

private:
  int state_;
};

void error_quit(std::string msg) {
  std::cout << msg << std::endl;
  exit(1);
}

#endif // !__LOMOT_REACTOR_UTILS__
