#ifndef __LOEVENT_HTTP_SERVER__
#define __LOEVENT_HTTP_SERVER__

// #include <cstddef>

#include <memory>

#include "event_loop.hpp"
#include "http/http_context.hpp"
#include "http/http_request.hpp"
#include "http/http_response.hpp"
#include "io_context.hpp"
#include "spdlog/spdlog.h"
#include "tcp_server.hpp"
#include "utils.hpp"

namespace loevent {
typedef std::function<void(const HttpRequest &, HttpResponse *)> HttpCallback;

class HttpServer {
 public:
  HttpServer(EventLoop &loop, int port)
      : tcpServer_(loop, port, "httpserver", 4096), port_(port) {
    tcpServer_.setConnectionCallback([](const TcpConnectionPtr &conn) {
      spdlog::info("[onConnection] fd: {}", conn->getFd());
      conn->setContext(std::make_shared<HttpContext>());
    });

    tcpServer_.setMessageCallback([this](const TcpConnectionPtr &conn) {
      // BufferPtr buffer = conn->getRecvBuffer();
      // HttpContext *contex = dynamic_cast<HttpContext *>(conn->getContext());
      auto context = std::dynamic_pointer_cast<HttpContext>(conn->getContext());
      HttpContextStatus status = context->parseHttpReq(conn->getRecvBuffer());
      if (status == HttpContextStatus::GotAll) {
        onRawReq(conn, context->getHttpRequest());
        context->reset();
      } else if (status == HttpContextStatus::Broken) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        tcpServer_.closeConnection(conn->getFd());
      }
    });
  }
  void setHttpCallback(const HttpCallback &cb) { httpCallback_ = cb; }
  void defaultHttpCallback(const HttpRequest &, HttpResponse *res) {
    res->setStatusCode(HttpResponse::k404NotFound);
    res->setStatusMessage("Not Found");
    res->setCloseConnection(true);
  }

 private:
  HttpCallback httpCallback_;
  TcpServer tcpServer_;
  int port_;
  void onRawReq(const TcpConnectionPtr &conn, const HttpRequest &req) {
    spdlog::debug("onRawReq");
    const std::string connection = req.getHeader("Connection");

    bool close = connection == "close" ||
                 (req.version() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    if (httpCallback_) {
      httpCallback_(req, &response);
    } else {
      defaultHttpCallback(req, &response);
    }

    BufferPtr sendBuffer = response.genBuffer();
    // sendBuffer->printInfo();
    conn->send(sendBuffer);
    if (response.getCloseConnection()) {
      tcpServer_.closeConnection(conn->getFd());
    }
  }
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_SERVER__
