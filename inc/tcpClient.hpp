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

    loop_.createChannel(
        sockfd_,
        [this]() {
          char recvBuf[maxMessageLen_];
          int n = recv(sockfd_, recvBuf, maxMessageLen_, 0);
          if (n > 0) {
            messageCallback_(conn_, recvBuf, n);
            // spdlog::info("[recvBuf] recv n: {}", n);
          } else if (n == 0) {
            spdlog::info("connection closed");
            loop_.closeChannel(sockfd_);
          } else if (n == -1) {
            spdlog::error("{}: {}", errno, strerror(errno));
            if (errno != EAGAIN && errno != EINTR) {
              loop_.closeChannel(sockfd_);
            }
          } else {
            spdlog::error("unknow error: func recv return {}", n);
            loop_.closeChannel(sockfd_);
          }
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
