#ifndef __LOMOT_REACTOR_EVENTLOOP__
#define __LOMOT_REACTOR_EVENTLOOP__

#include <stdlib.h>
#include <sys/epoll.h>

#include <map>
#include <string>

#include "utils.hpp"

#define MAX_EVENTS 1024
typedef std::shared_ptr<Socket> SocketPtr;

typedef std::map<int, SocketPtr> SocketMap;
// typedef std::vector<Socket*> SocketList;

class EventLoop {
 private:
  int maxEventNum_;
  // SocketList socketList_;
  SocketMap socketMap_;

 public:
  EventLoop(int maxEventNum);
  ~EventLoop();
  void loop();
  // void addEvent();
};

EventLoop::EventLoop(int maxEventNum) : maxEventNum_(maxEventNum) {}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
  struct epoll_event ev, events[maxEventNum_];
  int new_events, sock_conn_fd, epollfd;

  if ((epollfd = epoll_create(maxEventNum_)) < 0) {
    printf("Error creating epoll..\n");
    exit(1);
  }

  for (;;) {
    new_events = epoll_wait(epollfd, events, maxEventNum_, -1);
    if (new_events == -1) {
      error_quit("Error in epoll_wait..");
    }

    for (int i = 0; i < new_events; ++i) {
      int sockfd = events[i].data.fd;

      socketMap_[sockfd] = std::make_shared<Socket>(sockfd);

      if (events[i].events & EPOLLIN) {
      }
      if (events[i].events & EPOLLOUT) {
      }
    }
  }
}

#endif  // !__LOMOT_REACTOR_EVENTLOOP__
