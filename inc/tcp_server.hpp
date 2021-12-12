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

class TcpServer {
 public:
  ~TcpServer() {
    delete accepter_;
    delete tcpConnections_;
  }
  TcpServer(EventLoop &loop, int port, std::string name, int maxMessageLen = 1024)
      : name_(name), loop_(loop), maxMessageLen_(maxMessageLen) {
    accepter_ = new Accepter(port);
    int listenfd = accepter_->getFd();
    spdlog::debug("listenfd: {}", listenfd);
    tcpConnections_ = new ConnectionMap(true);
    loop_.createMainEvent(listenfd, std::bind(&TcpServer::onAcceptEvent, this, listenfd),
                          POLLIN | POLLRDHUP | POLLERR | POLLHUP);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

  void onAcceptEvent(int listenfd) {
    while (true) {
      int sockfd = accepter_->doAccept();
      if (sockfd == -1) {
        break;
      }
      if (sockfd % 200 == 0) {
        spdlog::info("doAccept {}", sockfd);
        loop_.debugPrinfInfo();
      }
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
    if (connectionCallback_) {
      // conn->setConnectionCallback(connectionCallback_);
      connectionCallback_(conn);
    }
    loop_.createIoEvent(sockfd, std::bind(&TcpConnection::onRecv, conn, sockfd, conn),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);
    conn->setCloseCallback(
        [this](const TcpConnectionPtr conn) { tcpConnections_->remove(conn->getFd()); });
    tcpConnections_->put(sockfd, conn);
    return conn;
  }

 private:
  EventLoop &loop_;
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  ConnectionMap *tcpConnections_;
  Accepter *accepter_;
  int maxMessageLen_;
  std::string name_;
  int nextConnId_;
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_SERVER__
