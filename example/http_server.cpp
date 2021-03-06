#include "http/http_server.hpp"

using namespace loevent;

void onRequest(const HttpRequest &req, HttpResponse *res) {
  res->setStatusCode(HttpResponse::k200Ok);
  res->setStatusMessage("Ok");
  // res->setCloseConnection(false);

  // res->setBody("Hello");
  res->setBody(
      "<html><head><title>This is title</title></head>"
      "<body><h1>Hello Loevent</h1>"
      "<p>request line: " +
      req.getReqLine() +
      "</p>"
      "<p>Method: " +
      req.methodString() +
      "</p>"
      "<p>Path: " +
      req.path() +
      "</p>"
      "<p>query: " +
      req.query() +
      "</p>"
      "</body></html>");
}
using namespace loevent;
int main(int argc, const char **argv) {
  // spdlog::set_level(spdlog::level::debug);
  if (argc < 3) {
    error_quit("Example: ./server <port> <threadNum>");
  }
  int port = strtol(argv[1], NULL, 10);
  int threadNum = strtol(argv[2], NULL, 10);

  EventLoop eventLoop(20480, threadNum);
  HttpServer httpServer(eventLoop, port);
  httpServer.setHttpCallback(onRequest);
  eventLoop.loop();

  return 0;
}
