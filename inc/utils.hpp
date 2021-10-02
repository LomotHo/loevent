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

void error_quit(std::string msg) {
  std::cout << msg << std::endl;
  exit(1);
}

#endif  // !__LOMOT_REACTOR_UTILS__
