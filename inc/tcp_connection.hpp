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

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop &loop, std::string connName, int sockfd, size_t bufferSize)
      : socket_(sockfd), connName_(connName), loop_(loop) {
    recvBuffePtr_ = std::make_shared<Buffer>(bufferSize);
    // sendBuffePtr_ = std::make_shared<Buffer>(bufferSize);
    sendBuffePtr_ = std::make_shared<Buffer>(4, 8);
  }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setCloseCallback(const MessageCallback &cb) { closeCallback_ = cb; }
  int send(std::string msg) { return send(msg.c_str(), msg.length()); }
  int send(Buffer &buf) { return send(buf.start(), buf.readableBytes()); }
  int send(BufferPtr buf) { return send(buf->start(), buf->readableBytes()); }

  int send(const char *msg, int len) {
    int fd = socket_.getFd();
    return socket_.send(msg, len);
    // if (sendBuffePtr_->readableBytes() == 0) {
    //   int nAlreadySend = 0;
    //   // nAlreadySend = socket_.nonBlockSend(msg, len);
    //   if (nAlreadySend < len) {
    //     spdlog::debug("total {} |AlreadySend {}", len, nAlreadySend);
    //     sendBuffePtr_->write(msg + nAlreadySend, len - nAlreadySend);
    //     loop_.createIoEvent(
    //         fd,
    //         [this, fd, nAlreadySend]() {
    //           auto buffer = sendBuffePtr_;
    //           int rb = buffer->readableBytes();
    //           spdlog::debug("send in OUT event {} | ", fd);
    //           if (rb == 0) {
    //             spdlog::debug("close OUT event");
    //             loop_.closeIoEvent(fd);
    //             return;
    //           }
    //           int n = socket_.nonBlockSend(buffer->start() + nAlreadySend, rb);
    //           if (n > 0) {
    //             buffer->retrieve(n);
    //           }
    //         },
    //         POLLOUT);
    //   }
    // } else {
    //   // FIX need lock
    //   sendBuffePtr_->write(msg, len);
    // }

    // sendBuffePtr_->write(msg, len);
    // while (true) {
    // }
    // if (errno != 0) {
    //   if (errno == EAGAIN) {
    //     spdlog::error("{}: EAGAIN | sockfd: {} | n: {}", errno, fd, n);
    //   } else if (errno == EINTR || errno == ECONNRESET || errno == ENOTCONN) {
    //     spdlog::error("{}: {} | sockfd: {} | n: {}", errno, strerror(errno), fd, n);
    //   } else {
    //     spdlog::error("{}: {} | sockfd: {} | n: {}", errno, strerror(errno), fd, n);
    //   }
    // }
    // if (n != -1 || (errno != EAGAIN && errno != EINTR)) {
    //   // closeConnection(fd);
    // }
  }

  int getFd() { return socket_.getFd(); }
  std::string getName() { return connName_; }
  ~TcpConnection() { spdlog::debug("connection destoried, fd: {}", getFd()); }
  BufferPtr getRecvBuffer() { return recvBuffePtr_; }
  void setContext(const IoContextPtr &context) { context_ = context; }
  IoContextPtr getContext() { return context_; }
  void closeConnection() {
    // FIX: need lock
    if (closeCallback_) {
      closeCallback_(shared_from_this());
    }
    loop_.closeIoEvent(socket_.getFd());
  }
  void onRecv(int fd, TcpConnectionPtr conn) {
    auto buffer = conn->getRecvBuffer();
    while (true) {
      int wb = buffer->writableBytes();
      if (wb == 0) {
        spdlog::info("write buffer full");
        closeConnection();
        break;
      }
      int n = recv(fd, buffer->end(), wb, MSG_DONTWAIT);
      spdlog::debug("recv n: {} | writableBytes: {}", n, wb);
      if (n > 0) {
        // if (!conn->getRecvBuffer()->write(recvBuf, n)) {
        //   spdlog::error("write buffer error, fd: {}", fd);
        // }
        buffer->manualWrite(n);
        if (messageCallback_) {
          messageCallback_(conn);
        }
        if (n < wb) {
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
        if (n != -1 || (errno != EAGAIN && errno != EINTR)) {
          closeConnection();
        }
        break;
      }
    }
  }

 private:
  // IoEvent &ioEvent_;
  Socket socket_;
  BufferPtr recvBuffePtr_;
  BufferPtr sendBuffePtr_;
  std::string connName_;
  EventLoop &loop_;
  IoContextPtr context_;
  MessageCallback messageCallback_;
  CloseCallback closeCallback_;
};

}  // namespace loevent
#endif  // !__LOEVENT_TCP_CONNECTION__
