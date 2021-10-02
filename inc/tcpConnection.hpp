#include <string>
#include <vector>

#include "eventLoop.hpp"
#include "utils.hpp"

class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd);
  void send(char *message, int len);
  // void setReadCallback(EventCallback cb) {
  //   channel_.setReadCallback(std::move(cb));
  // }
  // void setWriteCallback(EventCallback cb) {
  //   channel_.setWriteCallback(std::move(cb));
  // }

 private:
  // Channel &channel_;
  Socket socket_;
  std::string connName_;
  EventLoop &loop_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

TcpConnection::TcpConnection(EventLoop &loop, std::string connName, int sockfd)
    : socket_(sockfd), connName_(connName), loop_(loop) {}

void TcpConnection::send(char *message, int len) {
  socket_.socketSend(message, len);
  // std::vector<char> buf(message, message + len);
  // spdlog::debug("setWriteCallback");
  // channel_.setWriteCallback([this, buf]() { s.socketSend(buf); });
  // loop_.addChannel(
  //     s.sockfd, [this, buf]() { s.socketSend(buf); }, 1);
}
