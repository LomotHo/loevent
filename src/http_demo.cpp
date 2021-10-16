#include "event_loop.hpp"
#include "http_server.hpp"

using namespace loevent;
int main(int argc, const char** argv) {
  spdlog::set_level(spdlog::level::debug);
  if (argc < 2) {
    error_quit("Example: ./server [port]");
  }
  int port = strtol(argv[1], NULL, 10);

  EventLoop eventLoop(20480);
  HttpServer httpServer(eventLoop, 3000);
  eventLoop.loop();

  return 0;
}