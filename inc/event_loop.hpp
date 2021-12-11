#ifndef __LOEVENT_EVENTLOOP__
#define __LOEVENT_EVENTLOOP__

#include <errno.h>
#include <spdlog/spdlog.h>
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

class IoEventMap {
 public:
  IoEventMap(bool enableLock) : enableLock_(enableLock){};
  void put(const int &key, IoEventPtr value) {
    // if (enableLock_) std::lock_guard lock(mtx_);
    std::lock_guard lock(mtx_);
    map_.emplace(key, value);
  }
  int size() {
    // if (enableLock_) std::lock_guard lock(mtx_);
    std::lock_guard lock(mtx_);
    return map_.size();
  }
  std::optional<IoEventPtr> get(const int &key) {
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
  std::unordered_map<int, IoEventPtr> map_;
  std::shared_mutex mtx_;
  bool enableLock_;
};

class SingleEventLoop {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  IoEventMap *ioEventMap_;
  int epollfd_;
  struct epoll_event *tmpEvents_;
  int threadNum_;
  int id_;
  bool inThisLoop(int fd) {
    if (threadNum_ == 0) return true;
    return fd % threadNum_ == id_;
  }

 public:
  SingleEventLoop(int maxEventNum, int threadNum, int loopId);
  ~SingleEventLoop() {
    delete tmpEvents_;
    delete ioEventMap_;
  };
  void debugPrinfInfo() {
    spdlog::info("EventLoop{} size: {}", id_, ioEventMap_->size());
  }
  void loop();
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask);
  void removeEventFromLoop(int fd);
  void closeIoEvent(int fd);
  // void addEvent();
};

SingleEventLoop::SingleEventLoop(int maxEventNum, int threadNum, int loopId)
    : maxEventNum_(maxEventNum), threadNum_(threadNum), id_(loopId) {
  ioEventMap_ = new IoEventMap(threadNum > 0 ? true : false);
  tmpEvents_ = new struct epoll_event[maxEventNum];
  if ((epollfd_ = epoll_create(maxEventNum_)) < 0) {
    error_quit("Error creating epoll...");
  }
}

void SingleEventLoop::loop() {
  spdlog::info("EventLoop{} running...", id_);
  IoEventPtr defaultIoEvent = std::make_shared<IoEvent>();
  defaultIoEvent->setReadCallback([]() { spdlog::error("no ReadCallback"); });
  defaultIoEvent->setWriteCallback([]() { spdlog::error("no WriteCallback"); });
  for (;;) {
    int newEventNum = epoll_wait(epollfd_, tmpEvents_, maxEventNum_, -1);
    if (newEventNum == -1) {
      error_quit("Error in epoll_wait...");
    }
    for (int i = 0; i < newEventNum; ++i) {
      int sockfd = tmpEvents_[i].data.fd;
      if (tmpEvents_[i].events & EPOLLIN) {
        spdlog::debug("fd {} readable", sockfd);
        ioEventMap_->get(sockfd).value_or(defaultIoEvent)->readCallback();
      }
      if (tmpEvents_[i].events & EPOLLOUT) {
        spdlog::debug("fd {} writeable", sockfd);
        ioEventMap_->get(sockfd).value_or(defaultIoEvent)->writeCallback();
      }
    }
  }
}

void SingleEventLoop::closeIoEvent(int fd) {
  epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
  shutdown(fd, SHUT_RDWR);
  close(fd);
  ioEventMap_->remove(fd);
};

void SingleEventLoop::removeEventFromLoop(int fd) {
  epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
  ioEventMap_->remove(fd);
};

IoEventPtr SingleEventLoop::createIoEvent(int fd, EventCallback cb, uint32_t mask) {
  spdlog::debug("createIoEvent fd: {}", fd);

  IoEventPtr ioEvent;
  auto optIoEvent = ioEventMap_->get(fd);
  if (optIoEvent) {
    ioEvent = optIoEvent.value();
  } else {
    ioEvent = std::make_shared<IoEvent>();
    ioEventMap_->put(fd, ioEvent);
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
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    // error_quit("Error adding new event to epoll..");
    spdlog::error("Error adding new event to epoll, errno:{}, fd: {}", errno, fd);
  }
  return ioEvent;
}

// IoEventPtr SingleEventLoop::createReadEvent(int fd, EventCallback cb,) {
// }

class EventLoop {
 private:
  int maxEventNum_;
  int threadNum_;
  int fdToLoopId(int fd) {
    if (threadNum_ == 0) return 0;
    return fd % threadNum_;
  }
  std::vector<SingleEventLoop *> eventLoops_;

 public:
  EventLoop(int maxEventNum, int threadNum) : maxEventNum_(maxEventNum) {
    threadNum_ = threadNum > 0 ? threadNum : 0;
    for (size_t i = 0; i < threadNum_ + 1; i++) {
      eventLoops_.push_back(new SingleEventLoop(100, threadNum_, i));
    }
  };
  ~EventLoop(){};
  void debugPrinfInfo() {
    for (size_t i = 0; i < threadNum_ + 1; i++) {
      eventLoops_[i]->debugPrinfInfo();
    }
  }

  void loop() {
    std::thread t[threadNum_];
    for (size_t i = 0; i < threadNum_; i++) {
      t[i] = std::thread([this, i]() { eventLoops_[i]->loop(); });
    };
    // 主线程为loop[threadNum_]
    eventLoops_[threadNum_]->loop();
    for (size_t i = 0; i < threadNum_; i++) {
      t[i].join();
    }
  }
  IoEventPtr createIoEvent(int fd, EventCallback cb, uint32_t mask) {
    return eventLoops_[fdToLoopId(fd)]->createIoEvent(fd, cb, mask);
  }
  IoEventPtr createMainEvent(int fd, EventCallback cb, uint32_t mask) {
    return eventLoops_[threadNum_]->createIoEvent(fd, cb, mask);
  }
  void closeIoEvent(int fd) { eventLoops_[fdToLoopId(fd)]->closeIoEvent(fd); }
  void closeMainEvent(int fd) { eventLoops_[threadNum_]->closeIoEvent(fd); }
  void removeEventFromLoop(int fd) {
    eventLoops_[fdToLoopId(fd)]->removeEventFromLoop(fd);
  };
  void removeMainEventFromLoop(int fd) {
    eventLoops_[threadNum_]->removeEventFromLoop(fd);
  };
};

}  // namespace loevent

#endif  // !__LOEVENT_EVENTLOOP__
