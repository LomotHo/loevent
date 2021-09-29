#include "utils.hpp"
#include <string>

class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(std::string connName, int sockfd);
  Socket s;
  void send(const void *message, int len);

private:
  std::string connName_;
};

TcpConnection::TcpConnection(std::string connName, int sockfd)
    : s(sockfd), connName_(connName) {}

void TcpConnection::send(const void *message, int len) {
  s.socketSend(message, len);
}
