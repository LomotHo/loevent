#ifndef __LOMOT_REACTOR_CHANNEL__
#define __LOMOT_REACTOR_CHANNEL__

#include <sys/epoll.h>

#include <functional>

#include "spdlog/spdlog.h"

namespace loevent {

#define POLLIN EPOLLIN /* There is data to read.  */
// #define POLLPRI 0x002  /* There is urgent data to read.  */
#define POLLOUT EPOLLOUT /* Writing now will not block.  */
#define POLLERR EPOLLERR /* Error condition.  */
#define POLLHUP EPOLLHUP /* Hung up.  */
#define POLLRDHUP EPOLLRDHUP
// #define POLLNVAL 0x020 /* Invalid polling request.  */
#define POLLET EPOLLET

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
