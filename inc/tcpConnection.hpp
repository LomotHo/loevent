#include <string>
#include <vector>

#include "eventLoop.hpp"
#include "socket.hpp"

class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  // TcpConnection(EventLoop &loop, std::string connName, int sockfd);
  TcpConnection(EventLoop &loop, std::string connName, int sockfd)
      : socket_(sockfd), connName_(connName), loop_(loop) {}

  void send(void *msg, int len) {
    socket_.socketSend(msg, len);
    // std::vector<char> buf(message, message + len);
    // spdlog::debug("setWriteCallback");
    // channel_.setWriteCallback([this, buf]() { s.socketSend(buf); });
    // loop_.addChannel(
    //     s.sockfd, [this, buf]() { s.socketSend(buf); }, 1);
  }
  void send(std::vector<char> msg) {
    socket_.socketSend(msg.data(), msg.size());
  }

 private:
  // Channel &channel_;
  Socket socket_;
  std::string connName_;
  EventLoop &loop_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
