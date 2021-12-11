int TcpConnection::asyncSend(const char *msg, int len) {
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