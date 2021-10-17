#ifndef __LOEVENT_HTTP_SERVER__
#define __LOEVENT_HTTP_SERVER__

// #include <functional>

#include <cstddef>

#include "event_loop.hpp"
#include "http_context.hpp"
#include "http_request.hpp"
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
      conn->setHttpContext(HttpContext());
    });

    // status_ = HttpContextStatus::ExpectRequestLine;
    tcpServer_.setMessageCallback([this](const TcpConnectionPtr &conn) {
      // BufferPtr buffer = conn->getRecvBuffer();
      HttpContext *contex = conn->getHttpContext();
      contex->parseHttpReq(conn->getRecvBuffer());
      // printHexDump(buffer->start(), len);
      // conn->send(buffer->start(), len);
      conn->send("HTTP/1.0 200 OK\r\n");
      conn->send("{\"hello\":\"loevent\"}\r\n\r\n");
      tcpServer_.closeConnection(conn->getFd());
      // spdlog::debug("len: {}, crlf: 0x{}", len, crlf);
    });
  }

 private:
  TcpServer tcpServer_;
  int port_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_SERVER__
