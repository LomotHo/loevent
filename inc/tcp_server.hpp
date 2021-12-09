#ifndef __LOEVENT_TCP_SERVER__
#define __LOEVENT_TCP_SERVER__

#include <netinet/in.h>
#include <spdlog/spdlog.h>

#include <functional>
// #include <map>
#include <string>
#include <unordered_map>

#include "accepter.hpp"
#include "event_loop.hpp"
#include "tcp_connection.hpp"
#include "utils.hpp"

namespace loevent {

// typedef std::map<int, TcpConnectionPtr> ConnectionMap;
typedef std::unordered_map<int, TcpConnectionPtr> ConnectionMap;

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
    spdlog::debug("listenfd: {}", listenfd);
    loop_.createIoEvent(listenfd, std::bind(&TcpServer::onAceptEvent, this, listenfd),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
  // void closeConnection(int sockfd) {
  //   // FIX: need lock
  //   tcpConnections_.erase(sockfd);
  //   loop_.closeIoEvent(sockfd);
  // }
  void onAceptEvent(int listenfd) {
    while (true) {
      int sockfd = accepter_->doAccept();
      spdlog::info("doAccept {}", sockfd);
      if (sockfd == -1) {
        // spdlog::info("doAccept {}", sockfd);
        return;
      }
      fcntl(sockfd, F_SETFL, O_NONBLOCK);
      newConnection(sockfd);
    }
  }

  TcpConnectionPtr newConnection(int sockfd) {
    spdlog::debug("newConnection fd: {}", sockfd);
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;
    auto conn = std::make_shared<TcpConnection>(loop_, connName, sockfd, maxMessageLen_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback([this](const TcpConnectionPtr conn) {
      // int sockfd = conn->getFd();
      tcpConnections_.erase(conn->getFd());
    });

    loop_.createIoEvent(sockfd, std::bind(&TcpConnection::onRecv, conn, sockfd, conn),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);

    tcpConnections_[sockfd] = conn;
    if (connectionCallback_) {
      connectionCallback_(conn);
    }
    return conn;
  }
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_SERVER__
