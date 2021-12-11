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
    connector_->doConnect();
    sockfd_ = connector_->getFd();
    char connName[64];
    snprintf(connName, sizeof connName, "client-%d", sockfd_);
    conn_ = std::make_shared<TcpConnection>(loop_, connName, sockfd_, maxMessageLen);
    // conn_->setCloseCallback(
    //     [this](const TcpConnectionPtr conn) { spdlog::debug("close connection"); });
  }

  void setMessageCallback(const MessageCallback &cb) {
    messageCallback_ = cb;
    conn_->setMessageCallback(messageCallback_);
    loop_.createIoEvent(sockfd_, std::bind(&TcpConnection::onRecv, conn_, sockfd_, conn_),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);
  }
  // void setConnectedCallback(){};
  void send(std::string msg) { conn_->send(msg); }
  void send(char *msg, int len) { conn_->send(msg, len); }
  void closeConnection(int fd) { conn_->closeConnection(); }

 private:
  MessageCallback messageCallback_;
  Connector *connector_;
  EventLoop &loop_;
  TcpConnectionPtr conn_;
  int sockfd_;
  int maxMessageLen_;
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_CLIENT__
