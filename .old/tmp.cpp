#ifndef __LOEVENT_EVENTLOOP__
#define __LOEVENT_EVENTLOOP__

#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <thread>
#include <unordered_map>

// #include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

#include "io_event.hpp"
#include "utils.hpp"

namespace loevent {

typedef std::shared_ptr<IoEvent> IoEventPtr;
// typedef std::unordered_map<int, IoEventPtr> IoEventMap;
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

class EventLoop_ {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  IoEventMap ioEventMap_;
  int epollfd_;
  struct epoll_event *tmpEvents_;
  int id_;
  int threadNum_;
  bool inThisLoop(int fd) { return fd % threadNum_ == id_; }

 public:
  EventLoop_(int maxEventNum, int threadNum, int loopId);
  ~EventLoop_() { delete tmpEvents_; };
  void loop();
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask);
  void closeIoEvent(int fd);
  // void addEvent();
};

EventLoop_::EventLoop_(int maxEventNum, int threadNum, int loopId)
    : maxEventNum_(maxEventNum), id_(loopId), threadNum_(threadNum) {
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
        if (inThisLoop(sockfd)) {
          ioEventMap_.map_[sockfd]->readCallback();
        } else {
          ioEventMap_.mtx.lock();
          ioEventMap_.map_[sockfd]->readCallback();
          ioEventMap_.mtx.unlock();
        }
        // spdlog::debug("fd {} readCallback finished", sockfd);
      }
      if (tmpEvents_[i].events & EPOLLOUT) {
        spdlog::debug("fd {} writeable", sockfd);
        if (inThisLoop(sockfd)) {
          ioEventMap_.map_[sockfd]->writeCallback();
        } else {
          ioEventMap_.mtx.lock();
          ioEventMap_.map_[sockfd]->writeCallback();
          ioEventMap_.mtx.unlock();
        }
      }
      // ioEventMap_[sockfd] = std::make_shared<IoEvent>();
    }
  }
}

void EventLoop_::closeIoEvent(int fd) {
  epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
  shutdown(fd, SHUT_RDWR);
  close(fd);
  if (inThisLoop(fd)) {
    ioEventMap_.map_.erase(fd);
  } else {
    ioEventMap_.mtx.lock();
    ioEventMap_.map_.erase(fd);
    ioEventMap_.mtx.unlock();
  }
};

IoEventPtr EventLoop_::createIoEvent(int fd, EventCallback cb, uint32_t mask) {
  spdlog::debug("createIoEvent fd: {}", fd);

  IoEventPtr ioEvent;
  // using itType = decltype(ioEventMap_.map_.end());
  decltype(ioEventMap_.map_.end()) pos;
  decltype(ioEventMap_.map_.end()) end;
  if (inThisLoop(fd)) {
    ioEventMap_.map_[fd] = ioEvent;
  } else {
    ioEventMap_.mtx.lock();
    pos = ioEventMap_.map_.find(fd);
    end = ioEventMap_.map_.end();
    ioEventMap_.mtx.unlock();
  }

  if (pos == end) {
    ioEvent = std::make_shared<IoEvent>();
    if (inThisLoop(fd)) {
      ioEventMap_.map_[fd] = ioEvent;
    } else {
      ioEventMap_.mtx.lock();
      ioEventMap_.map_[fd] = ioEvent;
      ioEventMap_.mtx.unlock();
    }
  } else {
    ioEvent = pos->second;
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
  EventLoop(int maxEventNum, int threadNum)
      : maxEventNum_(maxEventNum), threadNum_(threadNum) {
    for (size_t i = 0; i < threadNum_; i++) {
      eventLoops_.push_back(new EventLoop_(100, threadNum_, i));
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
