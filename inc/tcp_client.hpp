#ifndef __LOEVENT_TCP_CLIENT__
#define __LOEVENT_TCP_CLIENT__

#include "connector.hpp"
#include "event_loop.hpp"
#include "tcp_connection.hpp"
#include "utils.hpp"

namespace loevent {

// typedef std::function<void(const TcpConnectionPtr, char *, int)> MessageCallback;

class TcpClient {
 public:
  ~TcpClient() { delete connector_; }
  TcpClient(EventLoop &loop, const char *ip, int port, int maxMessageLen)
      : loop_(loop), maxMessageLen_(maxMessageLen) {
    connector_ = new Connector(ip, port);
    sockfd_ = connector_->getFd();
  }

  void run() {
    int n = connector_->doConnect();
    if (n == 0) {
      newConnection(sockfd_);
    } else {
      spdlog::debug("{}: {} | sockfd: {} | n: {}", errno, strerror(errno), sockfd_, n);
      loop_.createMainEvent(
          sockfd_,
          [this]() {
            int n = connector_->doConnect();
            if (n < 0) {
              spdlog::debug("{}: {} | sockfd: {} | n: {}", errno, strerror(errno),
                            sockfd_, n);
              return;
            }
            loop_.removeMainEventFromLoop(sockfd_);
            spdlog::debug("sockfd{} connected", sockfd_);
            newConnection(sockfd_);
          },
          POLLOUT | POLLRDHUP | POLLERR | POLLHUP);
    }
  }

  TcpConnectionPtr newConnection(int sockfd) {
    char connName[64];
    snprintf(connName, sizeof connName, "client-%d", sockfd);
    conn_ = std::make_shared<TcpConnection>(loop_, connName, sockfd, maxMessageLen_);
    conn_->setMessageCallback(messageCallback_);
    // conn_->setCloseCallback(
    //     [this](const TcpConnectionPtr conn) { spdlog::debug("close connection");
    //     });
    loop_.createIoEvent(sockfd_, std::bind(&TcpConnection::onRecv, conn_, sockfd_),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);
    if (connectionCallback_) {
      // conn->setConnectionCallback(connectionCallback_);
      connectionCallback_(conn_);
    }
    return conn_;
  }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
  void closeConnection(int fd) { conn_->closeConnection(); }
  // void send(std::string msg) { conn_->send(msg); }
  // void send(char *msg, int len) { conn_->send(msg, len); }

 private:
  EventLoop &loop_;
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  int maxMessageLen_;
  Connector *connector_;
  TcpConnectionPtr conn_;
  int sockfd_;
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_CLIENT__
