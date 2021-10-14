#ifndef __LOEVENT_HTTP_SERVER__
#define __LOEVENT_HTTP_SERVER__

#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_server.hpp"
#include "utils.hpp"

namespace loevent {

class HttpServer {
 public:
  HttpServer(EventLoop &loop, int port)
      : tcpServer_(loop, port, "httpserver", 4096), port_(port) {}
  void onConnection(const TcpConnectionPtr &conn) {
    spdlog::info("[onConnection] fd: {}", conn->getFd());
  }

  void onMessage(const TcpConnectionPtr &conn, char *buf, int len) {
    printhexDump(buf, len);
    // std::string str("hello");
    // conn->send(str);
    conn->send(buf, len);
  }

 private:
  TcpServer tcpServer_;
  int port_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_SERVER__
