#ifndef __LOMOT_REACTOR_TCP_CLIENT__
#define __LOMOT_REACTOR_TCP_CLIENT__

#include "connector.hpp"
#include "eventLoop.hpp"
#include "tcpConnection.hpp"
#include "utils.hpp"

typedef std::function<void(const TcpConnectionPtr, char *, int)>
    MessageCallback;

class TcpClient {
 public:
  ~TcpClient() { delete connector_; }
  TcpClient(EventLoop &loop, const char *ip, int port, int maxMessageLen)
      : loop_(loop), maxMessageLen_(maxMessageLen) {
    connector_ = new Connector(ip, port);
    connector_->doConnect();
    sockfd_ = connector_->getFd();

    char connName[64];
    snprintf(connName, sizeof connName, "client-%d", sockfd_);

    conn_ = std::make_shared<TcpConnection>(loop_, connName, sockfd_);

    loop_.addChannel(
        sockfd_,
        [this]() {
          int n;
          char recvBuf[maxMessageLen_];
          if ((n = recv(sockfd_, recvBuf, maxMessageLen_, 0)) <= 0) {
            // epoll_ctl(epollfd_, EPOLL_CTL_DEL, sockfd, NULL);
            // shutdown(sockfd, SHUT_RDWR);
            spdlog::debug("error recv data");
          }
          messageCallback_(conn_, recvBuf, n);
        },
        0);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void send(std::string msg) { conn_->send(msg); }
  void send(void *msg, int len) { conn_->send(msg, len); }

 private:
  MessageCallback messageCallback_;
  Connector *connector_;
  EventLoop &loop_;
  TcpConnectionPtr conn_;
  int sockfd_;
  int maxMessageLen_;
};

#endif  // !__LOMOT_REACTOR_TCP_CLIENT__
