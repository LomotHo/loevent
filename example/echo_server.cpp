#include "spdlog/spdlog.h"
#include "tcp_server.hpp"

using namespace loevent;

void onConnection(const TcpConnectionPtr &conn) {
  // spdlog::debug("[onConnection] fd: {}", conn->getFd());
  int fd = conn->getFd();
  if (fd % 200 == 0) {
    spdlog::info("[onConnection] fd: {}", fd);
  }
}

void onMessage(const TcpConnectionPtr &conn) {
  auto buffer = conn->getRecvBuffer();
  int rb = buffer->readableBytes();
  // spdlog::info("[onMessage] recv len: {}", rb);
  // printHexDump(buffer->start(), rb);
  conn->send(buffer->start(), rb);
  buffer->retrieve(rb);
}

int main(int argc, char const *argv[]) {
  // spdlog::set_level(spdlog::level::debug);
  if (argc < 3) {
    error_quit("Example: ./server [port] [threadNum]");
  }
  int port = strtol(argv[1], NULL, 10);
  int threadNum = strtol(argv[2], NULL, 10);

  EventLoop eventLoop(20480, threadNum);
  spdlog::info("server running...");
  TcpServer tcpServer(eventLoop, port, "s233");
  tcpServer.setConnectionCallback(onConnection);
  tcpServer.setMessageCallback(onMessage);
  eventLoop.loop();
  return 0;
}
