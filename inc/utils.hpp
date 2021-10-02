#ifndef __LOMOT_REACTOR_UTILS__
#define __LOMOT_REACTOR_UTILS__

#include <sys/socket.h>

#include <cerrno>
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

void error_quit(std::string msg) {
  std::cout << msg << std::endl;
  std::cout << "errno: " << errno << std::endl;
  // printf("errno: %d\n", errno);
  exit(1);
}

void printhexDump(const char *buffer, size_t len) {
  if (buffer == NULL || len <= 0) {
    return;
  }
  printf("0x%p: [", buffer);
  for (size_t i = 0; i < len; i++) {
    printf("%.2X ", (unsigned char)buffer[i]);
  }
  printf("]\n");

  printf("data str: ");
  for (size_t i = 0; i < len; i++) {
    printf("%c", buffer[i]);
  }
}

void printBuf(const char *buffer, size_t len) {
  if (buffer == NULL || len <= 0) {
    return;
  }

  printf("data str: ");
  for (size_t i = 0; i < len; i++) {
    printf("%c", buffer[i]);
  }
  // printf("]\n");
}
#endif  // !__LOMOT_REACTOR_UTILS__
