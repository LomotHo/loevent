#ifndef __LOMOT_REACTOR_CHANNEL__
#define __LOMOT_REACTOR_CHANNEL__

#include <functional>

#include "spdlog/spdlog.h"

namespace loevent {

#define POLLIN 0x001 /* There is data to read.  */
// #define POLLPRI 0x002  /* There is urgent data to read.  */
#define POLLOUT 0x004 /* Writing now will not block.  */
#define POLLERR 0x008 /* Error condition.  */
#define POLLHUP 0x010 /* Hung up.  */
// #define POLLNVAL 0x020 /* Invalid polling request.  */
#define POLLET 1u << 31

// typedef std::function<void(void)> ReadEventCallback;
typedef std::function<void(void)> EventCallback;
class IoEvent {
 public:
  ~IoEvent() {}
  IoEvent() { spdlog::debug("new ioEvent"); }
  void setReadCallback(EventCallback cb) { readCallback = std::move(cb); }
  void setWriteCallback(EventCallback cb) { writeCallback = std::move(cb); }

  EventCallback readCallback;
  EventCallback writeCallback;

 private:
};

}  // namespace loevent
#endif  // !__LOMOT_REACTOR_CHANNEL__
