#ifndef __LOEVENT_HTTP_SERVER__
#define __LOEVENT_HTTP_SERVER__

// #include <functional>

#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_server.hpp"
#include "utils.hpp"

namespace loevent {

class HttpServer {
 public:
  HttpServer(EventLoop &loop, int port)
      : tcpServer_(loop, port, "httpserver", 4096), port_(port) {
    tcpServer_.setConnectionCallback([](const TcpConnectionPtr &conn) {
      spdlog::info("[onConnection] fd: {}", conn->getFd());
    });
    tcpServer_.setMessageCallback([](const TcpConnectionPtr &conn) {
      BufferPtr bt = conn->getRecvBuffer();
      int rb = bt->readableBytes();
      printHexDump(bt->start(), rb);

      conn->send(bt->start(), rb);
      bt->retrieve(rb);
    });
  }

 private:
  TcpServer tcpServer_;
  int port_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_SERVER__
