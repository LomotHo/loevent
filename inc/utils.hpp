#ifndef __LOEVENT_UTILS__
#define __LOEVENT_UTILS__

#include <sys/socket.h>

#include <cerrno>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace loevent {

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
  std::cout << "error_quit, errno: " << errno << std::endl;
  // printf("errno: %d\n", errno);
  exit(1);
}

void printHexDump(const char *buffer, size_t len) {
  if (buffer == NULL || len <= 0) {
    return;
  }
  printf("0x%p: [", buffer);
  for (size_t i = 0; i < len; i++) {
    printf("%.2X ", (unsigned char)buffer[i]);
  }
  printf("]\n");
}

void printBuf(const char *buffer, size_t len) {
  if (buffer == NULL || len <= 0) {
    return;
  }
  printf("[string content]: ");
  for (size_t i = 0; i < len; i++) {
    printf("%c", buffer[i]);
  }
  printf("\n");
}

template <class T>
class Map {
 public:
  Map(bool enableLock) : enableLock_(enableLock){};
  void put(const int &key, T value) {
    // if (enableLock_) std::lock_guard lock(mtx_);
    std::lock_guard lock(mtx_);
    map_.emplace(key, value);
  }
  int size() {
    // if (enableLock_) std::lock_guard lock(mtx_);
    std::lock_guard lock(mtx_);
    return map_.size();
  }
  std::optional<T> get(const int &key) {
    // if (enableLock_) std::shared_lock lock(mtx_);
    std::lock_guard lock(mtx_);
    auto it = map_.find(key);
    if (it != map_.end()) return it->second;
    return {};
  }

  bool remove(const int &key) {
    // if (enableLock_) std::lock_guard lock(mtx_);
    std::lock_guard lock(mtx_);
    auto n = map_.erase(key);
    return n;
  }

 private:
  std::unordered_map<int, T> map_;
  std::shared_mutex mtx_;
  bool enableLock_;
};

}  // namespace loevent
#endif  // !__LOEVENT_UTILS__
