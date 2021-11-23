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

    conn_ = std::make_shared<TcpConnection>(loop_, connName, sockfd_, 2 * maxMessageLen);

    loop_.createIoEvent(
        sockfd_,
        [this]() {
          auto buffer = conn_->getRecvBuffer();
          while (true) {
            int wb = buffer->writableBytes();
            if (wb == 0) {
              spdlog::info("write buffer full");
              closeConnection(sockfd_);
              break;
            }
            int n = recv(sockfd_, buffer->end(), wb, MSG_DONTWAIT);
            spdlog::debug("recv n: {} | writableBytes: {}", n, wb);
            if (n > 0) {
              buffer->manualWrite(n);
              messageCallback_(conn_);
              if (n < wb) {
                break;
              }
            } else {
              if (errno != 0) {
                if (errno == EAGAIN) {
                  spdlog::debug("{}: EAGAIN | sockfd: {} | n: {}", errno, sockfd_, n);
                } else if (errno == EINTR || errno == ECONNRESET || errno == ENOTCONN) {
                  spdlog::debug("{}: {} | sockfd: {} | n: {}", errno, strerror(errno),
                                sockfd_, n);
                } else {
                  spdlog::error("{}: {} | sockfd: {} | n: {}", errno, strerror(errno),
                                sockfd_, n);
                }
              }
              if (n != -1 || (errno != EAGAIN && errno != EINTR)) {
                closeConnection(sockfd_);
              }
              break;
            }
          }
        },
        POLLIN | POLLRDHUP | POLLERR | POLLHUP | POLLET);
  }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void send(std::string msg) { conn_->send(msg); }
  void send(void *msg, int len) { conn_->send(msg, len); }
  void closeConnection(int fd) { loop_.closeIoEvent(fd); }

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
