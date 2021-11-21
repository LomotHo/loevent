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
    loop_.createIoEvent(listenfd, std::bind(&TcpServer::onAceptEvent, this, listenfd),
                        POLLIN | POLLRDHUP | POLLERR | POLLHUP);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
  void closeConnection(int sockfd) {
    tcpConnections_.erase(sockfd);
    loop_.closeIoEvent(sockfd);
  }
  void onAceptEvent(int listenfd) {
    while (true) {
      int sockfd = accepter_->doAccept();
      if (sockfd == -1) {
        return;
      }
      fcntl(sockfd, F_SETFL, O_NONBLOCK);
      newConnection(sockfd);
    }
  }
  void onRecvEvent(int fd, TcpConnectionPtr conn) {
    auto buffer = conn->getRecvBuffer();
    while (true) {
      int wb = buffer->writableBytes();
      if (wb == 0) {
        spdlog::info("write buffer full");
        closeConnection(fd);
        break;
      }
      int n = recv(fd, buffer->end(), wb, 0);
      spdlog::debug("recv n: {} | writableBytes: {}", n, wb);
      if (n > 0) {
        // if (!conn->getRecvBuffer()->write(recvBuf, n)) {
        //   spdlog::error("write buffer error, fd: {}", fd);
        // }
        buffer->manualWrite(n);
        messageCallback_(conn);
        if (n < wb) {
          break;
        }
      } else {
        if (errno != 0) {
          if (errno == EAGAIN) {
            spdlog::debug("{}: EAGAIN | sockfd: {} | n: {}", errno, fd, n);
          } else if (errno == EINTR) {
            spdlog::debug("{}: EINTR | sockfd: {} | n: {}", errno, fd, n);
          } else {
            spdlog::error("{}: {} | sockfd: {} | n: {}", errno, strerror(errno), fd, n);
          }
        }
        if (n != -1 || (errno != EAGAIN && errno != EINTR)) {
          closeConnection(fd);
        }
        break;
      }
    }
  }

  TcpConnectionPtr newConnection(int sockfd) {
    spdlog::debug("newConnection fd: {}", sockfd);
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;
    auto conn = std::make_shared<TcpConnection>(loop_, connName, sockfd, maxMessageLen_);
    loop_.createIoEvent(sockfd, std::bind(&TcpServer::onRecvEvent, this, sockfd, conn),
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
