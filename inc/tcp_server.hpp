#ifndef __LOMOT_REACTOR_TCP_SERVER__
#define __LOMOT_REACTOR_TCP_SERVER__

#include <netinet/in.h>

#include <functional>
#include <map>
#include <string>

#include "accepter.hpp"
#include "event_loop.hpp"
#include "tcp_connection.hpp"
#include "utils.hpp"

namespace loevent {

typedef std::function<void(const TcpConnectionPtr, char *, int)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

class TcpServer {
 private:
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  ConnectionMap tcpConnections_;
  EventLoop &loop_;
  Accepter *accepter_;
  int maxMessageLen_;
  int nextConnId_;
  std::string name_;

 public:
  ~TcpServer() { delete accepter_; }
  TcpServer(EventLoop &loop, int port, std::string name, int maxMessageLen)
      : name_(name), loop_(loop), maxMessageLen_(maxMessageLen) {
    accepter_ = new Accepter(port);
    int listenfd = accepter_->getFd();
    loop_.createIoEvent(
        listenfd,
        [this, listenfd]() {
          spdlog::debug("[accept] listenfd: {}", listenfd);
          int sockfd = accepter_->doAccept();
          spdlog::info("[accept] sockfd: {}", sockfd);
          auto conn = newConnection(sockfd);
        },
        POLLIN | POLLRDHUP);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

  TcpConnectionPtr newConnection(int sockfd) {
    spdlog::debug("newConnection fd: {}", sockfd);
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;

    auto conn = std::make_shared<TcpConnection>(loop_, connName, sockfd);

    loop_.createIoEvent(
        sockfd,
        [this, conn, sockfd]() {
          char recvBuf[maxMessageLen_];
          int n = recv(sockfd, recvBuf, maxMessageLen_, 0);
          if (n > 0) {
            messageCallback_(conn, recvBuf, n);

            // spdlog::info("[recvBuf] recv n: {}", n);
          } else if (n == 0) {
            if (errno != 0) {
              spdlog::error("{}: {} | sockfd: {}", errno, strerror(errno), sockfd);
            }
            spdlog::info("connection closed, sockfd: {}", sockfd);
            loop_.closeIoEvent(sockfd);
          } else if (n == -1) {
            spdlog::error("{}: {} | sockfd: {}", errno, strerror(errno), sockfd);
            if (errno != EAGAIN && errno != EINTR) {
              loop_.closeIoEvent(sockfd);
            }
          } else {
            spdlog::error("unknow error: func recv return {}", n);
            loop_.closeIoEvent(sockfd);
          }
        },
        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);
    tcpConnections_[connName] = conn;
    if (connectionCallback_) {
      connectionCallback_(conn);
    }
    return conn;
  }
  // void start(int a) {}
};

}  // namespace loevent
#endif  // !__LOMOT_REACTOR_TCP_SERVER__
