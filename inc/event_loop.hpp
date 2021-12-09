#ifndef __LOEVENT_EVENTLOOP__
#define __LOEVENT_EVENTLOOP__

#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <thread>
#include <unordered_map>

// #include <map>
#include <string>
#include <vector>

#include "io_event.hpp"
#include "utils.hpp"

namespace loevent {

typedef std::shared_ptr<IoEvent> IoEventPtr;
typedef std::unordered_map<int, IoEventPtr> IoEventMap;

class EventLoop_ {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  IoEventMap ioEventMap_;
  int epollfd_;
  struct epoll_event *tmpEvents_;
  int id_;

 public:
  EventLoop_(int loopNum, int maxEventNum);
  ~EventLoop_() { delete tmpEvents_; };
  void loop();
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask);
  void closeIoEvent(int fd);
  // void addEvent();
};

EventLoop_::EventLoop_(int loopId, int maxEventNum)
    : maxEventNum_(maxEventNum), id_(loopId) {
  tmpEvents_ = new struct epoll_event[maxEventNum];
  if ((epollfd_ = epoll_create(maxEventNum_)) < 0) {
    error_quit("Error creating epoll...");
  }
}

void EventLoop_::loop() {
  spdlog::debug("EventLoop_ loop");

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

void EventLoop_::closeIoEvent(int fd) {
  epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
  shutdown(fd, SHUT_RDWR);
  close(fd);
  ioEventMap_.erase(fd);
};

IoEventPtr EventLoop_::createIoEvent(int fd, EventCallback cb, uint32_t mask) {
  spdlog::debug("createIoEvent fd: {}", fd);

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
    spdlog::debug("createIoEvent setReadCallback fd: {}", fd);
    ioEvent->setReadCallback(cb);
  } else if (mask & POLLOUT) {
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

class EventLoop {
 private:
  int maxEventNum_;
  int threadNum_;
  int fdToLoopId(int fd) { return fd % threadNum_; }
  std::vector<EventLoop_ *> eventLoops_;

 public:
  EventLoop(int threadNum, int maxEventNum)
      : maxEventNum_(maxEventNum), threadNum_(threadNum) {
    for (size_t i = 0; i < threadNum_; i++) {
      eventLoops_.push_back(new EventLoop_(i, 100));
    }
  };
  ~EventLoop(){};

  void loop() {
    std::thread t[threadNum_];
    for (size_t i = 0; i < threadNum_; i++) {
      t[i] = std::thread([this, i]() { eventLoops_[i]->loop(); });
    };
    for (size_t i = 0; i < threadNum_; i++) {
      t[i].join();
    }
  }
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask) {
    return eventLoops_[fdToLoopId(fd)]->createIoEvent(fd, cb, mask);
  }
  void closeIoEvent(int fd) { eventLoops_[fdToLoopId(fd)]->closeIoEvent(fd); }
};

}  // namespace loevent

#endif  // !__LOEVENT_EVENTLOOP__
