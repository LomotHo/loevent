#include <functional>

// #include "eventLoop.hpp"
#include "spdlog/spdlog.h"

// typedef std::function<void(void)> ReadEventCallback;
typedef std::function<void(void)> EventCallback;
class Channel {
 public:
  ~Channel() {}
  Channel() { spdlog::debug("new channel"); }
  void setReadCallback(EventCallback cb) { readCallback = std::move(cb); }
  void setWriteCallback(EventCallback cb) { writeCallback = std::move(cb); }

  EventCallback readCallback;
  EventCallback writeCallback;

 private:
};
