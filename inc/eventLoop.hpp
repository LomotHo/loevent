#ifndef __LOMOT_REACTOR_EVENTLOOP__
#define __LOMOT_REACTOR_EVENTLOOP__

#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include <map>
#include <string>

#include "channel.hpp"
#include "utils.hpp"

#define MAX_EVENTS 1024

typedef std::shared_ptr<Channel> ChannelPtr;
typedef std::map<int, ChannelPtr> ChannelMap;

class EventLoop {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  ChannelMap channelMap_;
  int epollfd_;
  struct epoll_event *tmpEvents_;

 public:
  EventLoop(int maxEventNum);
  ~EventLoop() { delete tmpEvents_; };
  void loop();
  ChannelPtr addChannel(int fd, EventCallback cb, int mask);
  // void addEvent();
};

EventLoop::EventLoop(int maxEventNum) : maxEventNum_(maxEventNum) {
  tmpEvents_ = new struct epoll_event[maxEventNum];
  if ((epollfd_ = epoll_create(maxEventNum_)) < 0) {
    error_quit("Error creating epoll...");
  }
}

// EventLoop::~EventLoop() {}

void EventLoop::loop() {
  // struct epoll_event ev, events[maxEventNum_];
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
        channelMap_[sockfd]->readCallback();
        spdlog::debug("fd {} readCallback finished", sockfd);

        // struct epoll_event ev;
        // ev.data.fd = sockfd;
        // ev.events = EPOLLOUT | EPOLLET;
        // epoll_ctl(epollfd_, EPOLL_CTL_MOD, sockfd, &ev);
      }
      if (tmpEvents_[i].events & EPOLLOUT) {
        spdlog::debug("fd {} writeable", sockfd);
        channelMap_[sockfd]->writeCallback();
      }
      // tmpEvents_[i].events

      // channelMap_[sockfd] = std::make_shared<Channel>();
    }
  }
}

ChannelPtr EventLoop::addChannel(int fd, EventCallback cb, int mask) {
  spdlog::debug("addChannel fd: {}", fd);

  // 是否已存在channel
  if (channelMap_.find(fd) == channelMap_.end()) {
    channelMap_[fd] = std::make_shared<Channel>();
  }

  struct epoll_event ev;
  if (mask == 0) {
    ev.events = EPOLLIN | EPOLLET;
    spdlog::debug("addChannel setReadCallback fd: {}", fd);

    channelMap_[fd]->setReadCallback(cb);
  } else if (mask == 1) {
    ev.events = EPOLLOUT | EPOLLET;
    spdlog::debug("addChannel setWriteCallback fd: {}", fd);
    channelMap_[fd]->setWriteCallback(cb);
  }
  ev.data.fd = fd;
  // if (channelMap_.find(fd) == channelMap_.end()) {
  // }
  if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    printf("errno: %d\n", errno);
    error_quit("Error adding new event to epoll..");
  }
  return channelMap_[fd];
  //  else {
  //   if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
  //     printf("errno: %d\n", errno);
  //     error_quit("Error mod epoll event..");
  //   }
  // }
}

#endif  // !__LOMOT_REACTOR_EVENTLOOP__
