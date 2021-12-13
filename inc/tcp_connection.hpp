#ifndef __LOEVENT_TCP_CONNECTION__
#define __LOEVENT_TCP_CONNECTION__

#include <spdlog/spdlog.h>

#include <cstddef>
#include <string>
#include <vector>

#include "buffer.hpp"
#include "event_loop.hpp"
#include "io_context.hpp"
#include "socket.hpp"

namespace loevent {

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr)> CloseCallback;
typedef loevent::Map<TcpConnectionPtr> ConnectionMap;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd, size_t bufferSize)
      : socket_(sockfd), connName_(connName), loop_(loop) {
    recvBuffePtr_ = std::make_shared<Buffer>(bufferSize);
    sendBuffePtr_ = std::make_shared<Buffer>(bufferSize);
    // recvBuffePtr_ = std::make_shared<Buffer>(4, 10);
    // sendBuffePtr_ = std::make_shared<Buffer>(4, 10);
  }
  ~TcpConnection() { spdlog::debug("connection destoried, fd: {}", getFd()); }
  bool connectionIsBroken() { return connectionBroken_; }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
  void setCloseCallback(const MessageCallback &cb) { closeCallback_ = cb; }
  int send(std::string msg) { return send(msg.c_str(), msg.length()); }
  int send(Buffer &buf) { return send(buf.start(), buf.readableBytes()); }
  int send(BufferPtr buf) { return send(buf->start(), buf->readableBytes()); }

  int send(const char *msg, int len) {
    int n = socket_.send(msg, len);
    if (n <= 0) {
      connectionBroken_ = true;
      // spdlog::debug("[conn send] {}: {} | sockfd: {} | n: {}", errno, strerror(errno),
      //               socket_.getFd(), n);
    }
    return n;
  }

  int getFd() { return socket_.getFd(); }
  std::string getName() { return connName_; }
  BufferPtr getRecvBuffer() { return recvBuffePtr_; }
  void setContext(const IoContextPtr &context) { context_ = context; }
  IoContextPtr getContext() { return context_; }
  void closeConnection() {
    if (closeCallback_) {
      closeCallback_(shared_from_this());
    }
    loop_.closeIoEvent(socket_.getFd());
  }
  void onRecv(int fd) {
    auto buffer = getRecvBuffer();
    while (true) {
      if (connectionIsBroken()) {
        closeConnection();
        return;
      }
      int wb = buffer->writableBytes();
      if (wb == 0) {
        spdlog::info("write buffer full");
        closeConnection();
        break;
      }
      int n = recv(fd, buffer->end(), wb, MSG_DONTWAIT);
      spdlog::debug("recv n: {} | writableBytes: {}", n, wb);
      if (n > 0) {
        buffer->manualWrite(n);
        if (messageCallback_) {
          messageCallback_(shared_from_this());
          spdlog::debug("messageCallback_ done");
        }
        if (n < wb) {
          spdlog::debug("no more data, break");
          break;
        }
      } else {
        if (errno != 0) {
          if (errno == EAGAIN) {
            spdlog::debug("{}: EAGAIN | sockfd: {} | n: {}", errno, fd, n);
          } else if (errno == EINTR || errno == ECONNRESET || errno == ENOTCONN) {
            spdlog::debug("{}: {} | sockfd: {} | n: {}", errno, strerror(errno), fd, n);
          } else {
            spdlog::error("{}: {} | sockfd: {} | n: {}", errno, strerror(errno), fd, n);
          }
        }
        // if (n != -1 || (errno != EAGAIN && errno != EINTR)) {
        if (n == 0 || (n == -1 && errno != EAGAIN && errno != EINTR)) {
          closeConnection();
          return;
        }
        break;
      }
    }
  }

 private:
  Socket socket_;
  BufferPtr recvBuffePtr_;
  BufferPtr sendBuffePtr_;
  std::string connName_;
  EventLoop &loop_;
  IoContextPtr context_;
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;
  CloseCallback closeCallback_;
  bool connectionBroken_ = false;
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_CONNECTION__
