#include "tcpConnection.hpp"
#include <functional>
#include <map>
#include <netinet/in.h>
#include <string>

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr, char *, int)>
    MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

class TcpServer {
private:
  /* data */
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  struct sockaddr_in servaddr_;
  ConnectionMap tcpConnections_;
  int nextConnId_;
  std::string name_;

public:
  ~TcpServer() {}
  TcpServer(int port, std::string name) : name_(name) {
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_.sin_port = htons(port);
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }
  void newConnection(int sockfd) {
    char buf[64];
    snprintf(buf, sizeof buf, "-%d", nextConnId_);
    nextConnId_++;
    std::string connName = name_ + buf;
    TcpConnectionPtr conn(new TcpConnection(connName, sockfd));
    tcpConnections_[connName] = conn;
  }
  void start(int a) {}
};
