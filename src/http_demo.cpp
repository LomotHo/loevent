#include "event_loop.hpp"
#include "http_server.hpp"

using namespace loevent;

void onRequest(const HttpRequest &req, HttpResponse *res) {
  res->setStatusCode(HttpResponse::k200Ok);
  res->setStatusMessage("Ok");
  res->setCloseConnection(true);
  // std::string now = Timestamp::now().toFormattedString();
  // std::string reqLine = req.getReqLine();
  res->setBody(
      "<html><head><title>This is title</title></head>"
      "<body><h1>Hello</h1><p>request line: " +
      req.getReqLine() + "</p></body></html>");
}
using namespace loevent;
int main(int argc, const char **argv) {
  spdlog::set_level(spdlog::level::debug);
  if (argc < 2) {
    error_quit("Example: ./server [port]");
  }
  int port = strtol(argv[1], NULL, 10);

  EventLoop eventLoop(20480);
  HttpServer httpServer(eventLoop, 3000);
  httpServer.setHttpCallback(onRequest);
  eventLoop.loop();

  return 0;
}