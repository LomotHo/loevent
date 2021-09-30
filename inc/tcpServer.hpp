#include <netinet/in.h>

#include <functional>
#include <map>
#include <string>

#include "eventLoop.hpp"
#include "tcpConnection.hpp"
#include "utils.hpp"

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr, char *, int)>
    MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

class TcpServer {
private:
  /* data */
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  struct sockaddr_in servaddr_;
  ConnectionMap tcpConnections_;
  int nextConnId_;
  std::string name_;
  int listenfd_;
  int maxMessageLen_;
  EventLoop &loop_;

public:
  ~TcpServer() {}
  TcpServer(EventLoop &loop, int port, std::string name, int maxMessageLen)
      : name_(name), loop_(loop), maxMessageLen_(maxMessageLen) {
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_.sin_port = htons(port);

    if ((listenfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      error_quit("Error creating socket..");
    }
    int on = 1;
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(listenfd_, (struct sockaddr *)&servaddr_, sizeof(servaddr_)) < 0) {
      error_quit("error on bind");
    }

    if (listen(listenfd_, 8) < 0) {
      error_quit("error on listen");
    }
    loop_.addChannel(
        listenfd_,
        [this]() {
          struct sockaddr_in cliaddr, servaddr;
          socklen_t clilen;
          struct epoll_event ev;
          int sock_conn_fd = accept4(listenfd_, (struct sockaddr *)&cliaddr,
                                     &clilen, SOCK_NONBLOCK);
          if (sock_conn_fd == -1) {
            error_quit("Error accepting new connection..");
          }
          auto conn =
              newConnection(sock_conn_fd, loop_.addChannel(
                                              sock_conn_fd, []() {}, 0));
          conn->setReadCallback([this, sock_conn_fd, conn]() {
            int n;
            char recvBuf[maxMessageLen_];
            if ((n = recv(sock_conn_fd, recvBuf, maxMessageLen_, 0)) <= 0) {
              // epoll_ctl(epollfd_, EPOLL_CTL_DEL, sockfd, NULL);
              // shutdown(sockfd, SHUT_RDWR);
              spdlog::debug("error recv data");
            }
            messageCallback_(conn, recvBuf, sizeof(recvBuf));
          });
        },

        0);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }

  TcpConnectionPtr newConnection(int sockfd, Channel &c) {
    spdlog::debug("newConnection fd: {}", sockfd);
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, c));
    tcpConnections_[connName] = conn;
    connectionCallback_(conn);
    return conn;
  }
  void start(int a) {}
};
void doAccept() {}