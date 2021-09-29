#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>

#include "spdlog/spdlog.h"

#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

void error_quit(std::string msg) {
  spdlog::error(msg);
  exit(1);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    error_quit("Please give a port number: ./server [port]");
  }
  int portno = strtol(argv[1], NULL, 10);
  int listenfd, connfd, sockfd;
  ssize_t n;
  struct sockaddr_in cliaddr, servaddr;
  socklen_t clilen;

  char buf[MAX_MESSAGE_LEN];
  memset(buf, 0, sizeof(buf));

  memset((char *)&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(portno);

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error_quit("Error creating socket..");
  }

  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    error_quit("error on bind");
  }

  if (listen(listenfd, 8) < 0) {
    error_quit("error on listen");
  }

  spdlog::info("listening on {}", portno);

  struct epoll_event ev, events[MAX_EVENTS];
  int new_events, sock_conn_fd, epollfd;
  // 创建epollfd
  if ((epollfd = epoll_create(MAX_EVENTS)) < 0) {
    error_quit("Error creating epoll..");
  }
  // 设置并添加 listenfd
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
    error_quit("Error adding new listeding socket to epoll..");
  }

  for (;;) {
    new_events = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (new_events == -1) {
      error_quit("Error in epoll_wait..");
    }

    for (int i = 0; i < new_events; ++i) {
      sockfd = events[i].data.fd;
      // 新连接到达
      if (sockfd == listenfd) {
        sock_conn_fd = accept4(listenfd, (struct sockaddr *)&cliaddr, &clilen,
                               SOCK_NONBLOCK);
        if (sock_conn_fd == -1) {
          error_quit("Error accepting new connection..");
        }

        // ev.events = EPOLLIN | EPOLLET;
        ev.events = EPOLLIN;
        ev.data.fd = sock_conn_fd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_conn_fd, &ev) == -1) {
          error_quit("Error adding new event to epoll..");
        }
      }
      // socket可读
      else if (events[i].events & EPOLLIN) {
        // n = recv(sockfd, buf, MAX_MESSAGE_LEN, 0);
        if ((n = recv(sockfd, buf, MAX_MESSAGE_LEN, 0)) <= 0) {
          epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
          shutdown(sockfd, SHUT_RDWR);
        }
        // ev.data.ptr = md;     //md为自定义类型，添加数据
        //修改标识符，等待下一个循环时发送数据，异步处理的精髓
        ev.events = EPOLLOUT | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
      }
      //有数据待发送，写socket
      else if (events[i].events & EPOLLOUT) {
        // struct myepoll_data *md = (myepoll_data *)events[i].data.ptr;
        // //取数据 sockfd = md->fd; send(sockfd, md->ptr, strlen((char
        // *)md->ptr), 0);  //发送数据
        send(sockfd, buf, n, 0);
        ev.data.fd = sockfd;
        ev.events = EPOLLIN | EPOLLET;
        //修改标识符，等待下一个循环时接收数据
        epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
      } else {
        spdlog::info("free loop");
      }
    }
  }
}
