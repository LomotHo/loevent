#ifndef __LOMOT_REACTOR_CHANNEL__
#define __LOMOT_REACTOR_CHANNEL__

#include <functional>

#include "spdlog/spdlog.h"

namespace loevent {

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

}  // namespace loevent
#endif  // !__LOMOT_REACTOR_CHANNEL__
