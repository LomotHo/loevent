#ifndef __LOEVENT_TCP_SERVER__
#define __LOEVENT_TCP_SERVER__

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
typedef std::map<int, TcpConnectionPtr> ConnectionMap;

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
    loop_.createIoEvent(listenfd, std::bind(&TcpServer::onAceptEvent, this, listenfd),
                        POLLIN | POLLRDHUP);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
  void closeConnection(int sockfd) {
    tcpConnections_.erase(sockfd);
    loop_.closeIoEvent(sockfd);
  }
  void onAceptEvent(int listenfd) {
    spdlog::debug("[accept] listenfd: {}", listenfd);
    int sockfd = accepter_->doAccept();
    spdlog::info("[accept] sockfd: {}", sockfd);
    // auto conn = newConnection(sockfd);
    newConnection(sockfd);
  }
  void onRecvEvent(int fd, TcpConnectionPtr conn) {
    char recvBuf[maxMessageLen_];
    int n = recv(fd, recvBuf, maxMessageLen_, 0);
    if (n > 0) {
      messageCallback_(conn, recvBuf, n);
    } else if (n == 0) {
      if (errno != 0) {
        spdlog::error("{}: {} | sockfd: {}", errno, strerror(errno), fd);
      }
      spdlog::info("connection closed, sockfd: {}", fd);
      closeConnection(fd);
    } else if (n == -1) {
      spdlog::error("{}: {} | sockfd: {}", errno, strerror(errno), fd);
      if (errno != EAGAIN && errno != EINTR) {
        closeConnection(fd);
      }
    } else {
      spdlog::error("unknow error: func recv return {}", n);
      closeConnection(fd);
    }
  }

  TcpConnectionPtr newConnection(int sockfd) {
    spdlog::debug("newConnection fd: {}", sockfd);
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;
    auto conn = std::make_shared<TcpConnection>(loop_, connName, sockfd);
    loop_.createIoEvent(sockfd, std::bind(&TcpServer::onRecvEvent, this, sockfd, conn),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);

    tcpConnections_[sockfd] = conn;
    if (connectionCallback_) {
      connectionCallback_(conn);
    }
    return conn;
  }
  // void start(int a) {}
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_SERVER__
