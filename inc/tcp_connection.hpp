#ifndef __LOMOT_REACTOR_TCP_CONNECTION__
#define __LOMOT_REACTOR_TCP_CONNECTION__

#include <string>
#include <vector>

#include "event_loop.hpp"
#include "socket.hpp"

namespace loevent {

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd)
      : socket_(sockfd), connName_(connName), loop_(loop) {}

  void send(std::string msg) { socket_.socketSend(msg.c_str(), msg.length()); }
  void send(void *msg, int len) { socket_.socketSend(msg, len); }
  // void send(std::vector<char> msg) {
  //   socket_.socketSend(msg.data(), msg.size());
  // }
  int getFd() { return socket_.getFd(); }
  std::string getName() { return connName_; }

 private:
  // IoEvent &ioEvent_;
  Socket socket_;
  std::string connName_;
  EventLoop &loop_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}  // namespace loevent
#endif  // !__LOMOT_REACTOR_TCP_CONNECTION__
