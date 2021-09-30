#include "utils.hpp"
#include <string>
#include <vector>

class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd,
                Channel &channel);
  Socket s;
  void send(char *message, int len);
  void setReadCallback(EventCallback cb) {
    channel_.setReadCallback(std::move(cb));
  }
  void setWriteCallback(EventCallback cb) {
    channel_.setWriteCallback(std::move(cb));
  }

private:
  Channel &channel_;
  std::string connName_;
  EventLoop &loop_;
};

TcpConnection::TcpConnection(EventLoop &loop, std::string connName, int sockfd,
                             Channel &channel)
    : s(sockfd), connName_(connName), loop_(loop), channel_(channel) {}

void TcpConnection::send(char *message, int len) {
  std::vector<char> buf(message, message + len);
  channel_.setWriteCallback([this, buf]() { s.socketSend(buf); });
  // loop_.addChannel(
  //     s.sockfd, [this, buf]() { s.socketSend(buf); }, 1);
}
