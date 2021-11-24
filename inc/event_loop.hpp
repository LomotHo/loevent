#ifndef __LOEVENT_EVENTLOOP__
#define __LOEVENT_EVENTLOOP__

#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <unordered_map>

// #include <map>
#include <string>

#include "io_event.hpp"
#include "utils.hpp"

namespace loevent {

typedef std::shared_ptr<IoEvent> IoEventPtr;
typedef std::unordered_map<int, IoEventPtr> IoEventMap;

class EventLoop {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  IoEventMap ioEventMap_;
  int epollfd_;
  struct epoll_event *tmpEvents_;

 public:
  EventLoop(int maxEventNum);
  ~EventLoop() { delete tmpEvents_; };
  void loop();
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask);
  void closeIoEvent(int fd);
  // void addEvent();
};

EventLoop::EventLoop(int maxEventNum) : maxEventNum_(maxEventNum) {
  tmpEvents_ = new struct epoll_event[maxEventNum];
  if ((epollfd_ = epoll_create(maxEventNum_)) < 0) {
    error_quit("Error creating epoll...");
  }
}

void EventLoop::loop() {
  spdlog::debug("EventLoop loop");

  for (;;) {
    int newEventNum = epoll_wait(epollfd_, tmpEvents_, maxEventNum_, -1);
    if (newEventNum == -1) {
      error_quit("Error in epoll_wait...");
    }

    for (int i = 0; i < newEventNum; ++i) {
      int sockfd = tmpEvents_[i].data.fd;
      if (tmpEvents_[i].events & EPOLLIN) {
        spdlog::debug("fd {} readable", sockfd);
        ioEventMap_[sockfd]->readCallback();
        // spdlog::debug("fd {} readCallback finished", sockfd);
      }
      if (tmpEvents_[i].events & EPOLLOUT) {
        spdlog::debug("fd {} writeable", sockfd);
        ioEventMap_[sockfd]->writeCallback();
      }
      // ioEventMap_[sockfd] = std::make_shared<IoEvent>();
    }
  }
}

void EventLoop::closeIoEvent(int fd) {
  epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
  shutdown(fd, SHUT_RDWR);
  close(fd);
  ioEventMap_.erase(fd);
};

IoEventPtr EventLoop::createIoEvent(int fd, EventCallback cb, uint32_t mask) {
  spdlog::debug("createIoEvent fd: {}", fd);

  // 是否已存在ioEvent
  // auto ioEvent = ioEventMap_.insert_or_assign(
  //     IoEventMap::value_type(fd, std::make_shared<IoEvent>()));

  IoEventPtr ioEvent;
  auto it = ioEventMap_.find(fd);
  if (it == ioEventMap_.end()) {
    ioEvent = std::make_shared<IoEvent>();
    ioEventMap_[fd] = ioEvent;
  } else {
    ioEvent = it->second;
  }

  struct epoll_event ev;
  ev.events = mask;

  if (mask & POLLIN) {
    // ev.events = EPOLLIN | EPOLLET;
    // ev.events = EPOLLIN;
    spdlog::debug("createIoEvent setReadCallback fd: {}", fd);

    ioEvent->setReadCallback(cb);
  } else if (mask & POLLOUT) {
    // ev.events = EPOLLOUT | EPOLLET;
    // ev.events = EPOLLOUT;
    spdlog::debug("createIoEvent setWriteCallback fd: {}", fd);
    ioEvent->setWriteCallback(cb);
  }
  ev.data.fd = fd;
  // if (ioEventMap_.find(fd) == ioEventMap_.end()) {
  // }
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("errno: %d\n", errno);
    error_quit("Error adding new event to epoll..");
  }
  return ioEvent;
}

}  // namespace loevent

#endif  // !__LOEVENT_EVENTLOOP__
