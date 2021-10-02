#include "eventLoop.hpp"
#include "spdlog/spdlog.h"
#include "tcpServer.hpp"
#include "utils.hpp"

void onConnection(const TcpConnectionPtr &conn) {
  spdlog::info("[onConnection] fd: {}", conn->getFd());
}

void onMessage(const TcpConnectionPtr &conn, char *buf, int len) {
  spdlog::info("[onMessage] recv len: {}", len);
  printhexDump(buf, len);
  conn->send(buf, len);
}

int main(int argc, char const *argv[]) {
  // spdlog::set_level(spdlog::level::debug);
  if (argc < 2) {
    error_quit("Please give a port number: ./server [port]");
  }
  int port = strtol(argv[1], NULL, 10);

  EventLoop eventLoop(2048);
  spdlog::info("server running...");
  TcpServer tcpServer(eventLoop, port, "s233", 4094);
  tcpServer.setConnectionCallback(onConnection);
  tcpServer.setMessageCallback(onMessage);
  eventLoop.loop();
  return 0;
}
