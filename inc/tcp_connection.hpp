#ifndef __LOEVENT_TCP_CONNECTION__
#define __LOEVENT_TCP_CONNECTION__

#include <cstddef>
#include <string>
#include <vector>

#include "buffer.hpp"
#include "event_loop.hpp"
#include "io_context.hpp"
#include "socket.hpp"

namespace loevent {

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd, size_t bufferSize)
      : socket_(sockfd), connName_(connName), loop_(loop) {
    // recvBuffePtr_ = std::make_shared<Buffer>(128);
    recvBuffePtr_ = std::make_shared<Buffer>(bufferSize);
    sendBuffePtr_ = std::make_shared<Buffer>(bufferSize);
  }

  void send(std::string msg) { socket_.send(msg.c_str(), msg.length()); }
  void send(void *msg, int len) { socket_.send(msg, len); }
  void send(Buffer &buf) { socket_.send(buf.start(), buf.readableBytes()); }
  void send(BufferPtr buf) { socket_.send(buf->start(), buf->readableBytes()); }
  // void send(std::vector<char> msg) {
  //   socket_.send(msg.data(), msg.size());
  // }
  int getFd() { return socket_.getFd(); }
  std::string getName() { return connName_; }
  ~TcpConnection() { spdlog::debug("connection destoried, fd: {}", getFd()); }
  BufferPtr getRecvBuffer() { return recvBuffePtr_; }
  void setContext(const IoContextPtr &context) { context_ = context; }
  IoContextPtr getContext() { return context_; }

 private:
  // IoEvent &ioEvent_;
  Socket socket_;
  BufferPtr recvBuffePtr_;
  BufferPtr sendBuffePtr_;
  std::string connName_;
  EventLoop &loop_;
  IoContextPtr context_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
}  // namespace loevent
#endif  // !__LOEVENT_TCP_CONNECTION__
