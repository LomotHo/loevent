#include "spdlog/spdlog.h"
#include "utils.hpp"

class Socket : noncopyable {
 public:
  Socket(int fd) : sockfd(fd) {}
  // ~Socket() {}
  int sockfd;
  // void socketSend(std::vector<char> msg) {
  //   spdlog::debug("socketSend {}", msg.data());
  //   // char *temp_ptr = msg.empty() ? 0 : &msg[0];
  //   char *str = new char[msg.size() + 1];
  //   copy(msg.begin(), msg.end(), str);
  //   str[msg.size()] = 0;

  //   send(sockfd, str, msg.size() + 1, 0);
  // }
  void socketSend(void *buf, int len) {
    spdlog::debug("socketSend2 {} | fd: {}", buf, sockfd);
    send(sockfd, buf, len, 0);
  }

 private:
  int state_;
};