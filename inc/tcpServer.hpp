// #include <fcntl.h>
#include <netinet/in.h>

#include <functional>
#include <map>
#include <string>

#include "accepter.hpp"
#include "eventLoop.hpp"
#include "tcpConnection.hpp"
#include "utils.hpp"

typedef std::function<void(const TcpConnectionPtr, char *, int)>
    MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

class TcpServer {
 private:
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;

  ConnectionMap tcpConnections_;
  int nextConnId_;
  std::string name_;
  int maxMessageLen_;
  EventLoop &loop_;
  Accepter *accepter_;

 public:
  ~TcpServer() { delete accepter_; }
  TcpServer(EventLoop &loop, int port, std::string name, int maxMessageLen)
      : name_(name), loop_(loop), maxMessageLen_(maxMessageLen) {
    accepter_ = new Accepter(port);
    int listenfd = accepter_->getListenFd();
    loop_.addChannel(
        listenfd,
        [this, listenfd]() {
          spdlog::debug("[accept] listenfd: {}", listenfd);
          int sockfd = accepter_->doAccept();
          auto conn = newConnection(sockfd);
        },
        0);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }

  TcpConnectionPtr newConnection(int sockfd) {
    spdlog::debug("newConnection fd: {}", sockfd);
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;

    auto conn = std::make_shared<TcpConnection>(loop_, connName, sockfd);

    loop_.addChannel(
        sockfd,
        [this, conn, sockfd]() {
          int n;
          char recvBuf[maxMessageLen_];
          if ((n = recv(sockfd, recvBuf, maxMessageLen_, 0)) <= 0) {
            // epoll_ctl(epollfd_, EPOLL_CTL_DEL, sockfd, NULL);
            // shutdown(sockfd, SHUT_RDWR);
            spdlog::debug("error recv data");
          }
          messageCallback_(conn, recvBuf, n);
          // spdlog::info("[recvBuf] recv n: {}", n);
        },
        0);
    tcpConnections_[connName] = conn;
    if (connectionCallback_) {
      connectionCallback_(conn);
    }
    return conn;
  }
  void start(int a) {}
};
