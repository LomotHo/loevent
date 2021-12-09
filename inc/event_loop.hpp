#ifndef __LOEVENT_EVENTLOOP__
#define __LOEVENT_EVENTLOOP__

#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "io_event.hpp"
#include "utils.hpp"

namespace loevent {

typedef std::shared_ptr<IoEvent> IoEventPtr;
// typedef std::unordered_map<int, IoEventPtr> IoEventMap;
// typedef struct IoEventMap {
//   std::unordered_map<int, IoEventPtr> map_;
//   std::mutex mtx;
// } IoEventMap;

class IoEventMap {
 public:
  IoEventMap(){};
  void put(const int &key, IoEventPtr value) {
    std::lock_guard lock(mtx_);
    map_.emplace(key, value);
  }

  std::optional<IoEventPtr> get(const int &key) {
    std::shared_lock lock(mtx_);
    auto it = map_.find(key);
    if (it != map_.end()) return it->second;
    return {};
  }

  bool remove(const int &key) {
    std::lock_guard lock(mtx_);
    auto n = map_.erase(key);
    return n;
  }

 private:
  std::unordered_map<int, IoEventPtr> map_;
  std::shared_mutex mtx_;
};

class SingleEventLoop {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  IoEventMap ioEventMap_;
  int epollfd_;
  struct epoll_event *tmpEvents_;
  int threadNum_;
  int id_;
  bool inThisLoop(int fd) { return fd % threadNum_ == id_; }

 public:
  SingleEventLoop(int maxEventNum, int threadNum, int loopId);
  ~SingleEventLoop() { delete tmpEvents_; };
  void loop();
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask);
  void closeIoEvent(int fd);
  // void addEvent();
};

SingleEventLoop::SingleEventLoop(int maxEventNum, int threadNum, int loopId)
    : maxEventNum_(maxEventNum), threadNum_(threadNum), id_(loopId) {
  tmpEvents_ = new struct epoll_event[maxEventNum];
  if ((epollfd_ = epoll_create(maxEventNum_)) < 0) {
    error_quit("Error creating epoll...");
  }
}

void SingleEventLoop::loop() {
  spdlog::debug("SingleEventLoop loop");

  for (;;) {
    int newEventNum = epoll_wait(epollfd_, tmpEvents_, maxEventNum_, -1);
    if (newEventNum == -1) {
      error_quit("Error in epoll_wait...");
    }

    for (int i = 0; i < newEventNum; ++i) {
      int sockfd = tmpEvents_[i].data.fd;
      if (tmpEvents_[i].events & EPOLLIN) {
        spdlog::debug("fd {} readable", sockfd);
        ioEventMap_.get(sockfd).value()->readCallback();
        // spdlog::debug("fd {} readCallback finished", sockfd);
      }
      if (tmpEvents_[i].events & EPOLLOUT) {
        spdlog::debug("fd {} writeable", sockfd);
        ioEventMap_.get(sockfd).value()->readCallback();
      }
      // ioEventMap_[sockfd] = std::make_shared<IoEvent>();
    }
  }
}

void SingleEventLoop::closeIoEvent(int fd) {
  epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
  shutdown(fd, SHUT_RDWR);
  close(fd);
  ioEventMap_.remove(fd);
};

IoEventPtr SingleEventLoop::createIoEvent(int fd, EventCallback cb, uint32_t mask) {
  spdlog::debug("createIoEvent fd: {}", fd);

  IoEventPtr ioEvent;
  auto optIoEvent = ioEventMap_.get(fd);
  if (optIoEvent) {
    ioEvent = optIoEvent.value();
  } else {
    ioEvent = std::make_shared<IoEvent>();
    ioEventMap_.put(fd, ioEvent);
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
  std::vector<SingleEventLoop *> eventLoops_;

 public:
  EventLoop(int maxEventNum, int threadNum, int pl)
      : maxEventNum_(maxEventNum), threadNum_(threadNum) {
    for (size_t i = 0; i < threadNum_; i++) {
      eventLoops_.push_back(new SingleEventLoop(100, threadNum_, i));
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
