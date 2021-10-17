#ifndef __LOEVENT_TCP_CONNECTION__
#define __LOEVENT_TCP_CONNECTION__

#include <string>
#include <vector>

#include "buffer.hpp"
#include "event_loop.hpp"
#include "http_context.hpp"
#include "socket.hpp"

namespace loevent {

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd)
      : socket_(sockfd), connName_(connName), loop_(loop) {
    recvBuffePtr_ = std::make_shared<Buffer>(16384);
  }

  void send(std::string msg) { socket_.socketSend(msg.c_str(), msg.length()); }
  void send(void *msg, int len) { socket_.socketSend(msg, len); }
  void send(Buffer &buf) { socket_.socketSend(buf.start(), buf.readableBytes()); }
  void send(BufferPtr buf) { socket_.socketSend(buf->start(), buf->readableBytes()); }
  // void send(std::vector<char> msg) {
  //   socket_.socketSend(msg.data(), msg.size());
  // }
  int getFd() { return socket_.getFd(); }
  std::string getName() { return connName_; }
  ~TcpConnection() { spdlog::debug("connection destoried, fd: {}", getFd()); }
  BufferPtr getRecvBuffer() { return recvBuffePtr_; }
  void setHttpContext(const HttpContext &context) { context_ = context; }
  HttpContext *getHttpContext() { return &context_; }

 private:
  // IoEvent &ioEvent_;
  Socket socket_;
  BufferPtr recvBuffePtr_;
  std::string connName_;
  EventLoop &loop_;
  HttpContext context_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
}  // namespace loevent
#endif  // !__LOEVENT_TCP_CONNECTION__
