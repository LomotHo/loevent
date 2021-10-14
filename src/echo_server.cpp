#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_server.hpp"
#include "utils.hpp"

using namespace loevent;

void onConnection(const TcpConnectionPtr &conn) {
  spdlog::info("[onConnection] fd: {}", conn->getFd());
}

void onMessage(const TcpConnectionPtr &conn) {
  // spdlog::info("[onMessage] recv len: {}", len);
  // printhexDump(buf, len);
  // std::string str("hello");
  // conn->send(str);
  // if (conn->getRecvBuffer()->readableBytes()) {
  // }
  int rb = conn->getRecvBuffer()->readableBytes();
  conn->send(conn->getRecvBuffer()->start(), rb);
  conn->getRecvBuffer()->retrieve(rb);
}

int main(int argc, char const *argv[]) {
  // spdlog::set_level(spdlog::level::debug);
  if (argc < 2) {
    error_quit("Example: ./server [port]");
  }
  int port = strtol(argv[1], NULL, 10);

  EventLoop eventLoop(20480);
  spdlog::info("server running...");
  TcpServer tcpServer(eventLoop, port, "s233", 4096);
  tcpServer.setConnectionCallback(onConnection);
  tcpServer.setMessageCallback(onMessage);
  eventLoop.loop();
  return 0;
}
