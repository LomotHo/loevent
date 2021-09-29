#include "tcpConnection.hpp"

// #include "tcpConnection.hpp"
// #include <memory>
// #include <sys/socket.h>

// class noncopyable {
// public:
//   noncopyable(const noncopyable &) = delete;
//   void operator=(const noncopyable &) = delete;

// protected:
//   noncopyable() = default;
//   ~noncopyable() = default;
// };

// class Socket {
// public:
//   Socket(int fd) : sockfd(fd) {}
//   ~Socket() {}
//   int sockfd;
//   void socketSend(const void *message, int len) {
//     send(sockfd, message, len, 0);
//   }

// private:
//   int state_;
// };

// class TcpConnection : noncopyable,
//                       public std::enable_shared_from_this<TcpConnection> {
// public:
//   TcpConnection(int sockfd) : s(sockfd) {}
//   Socket s;
//   void send(const void *message, int len) { s.socketSend(message, len); }

// private:
// };
