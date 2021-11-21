#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_client.hpp"
#include "utils.hpp"

using namespace loevent;

void onMessage(const TcpConnectionPtr &conn) {
  // spdlog::info("[onMessage] recv len: {}", len);
  // printHexDump(buf, len);
  // std::string str("hello");
  // conn->send(str);
  auto buffer = conn->getRecvBuffer();
  int rb = buffer->readableBytes();
  conn->send(buffer->start(), rb);
  buffer->retrieve(rb);
}

int main(int argc, char const *argv[]) {
  // spdlog::set_level(spdlog::level::debug);
  if (argc < 3) {
    error_quit("Example: ./client [ip] [port]");
  }
  const char *ip = argv[1];
  int port = strtol(argv[2], NULL, 10);

  EventLoop eventLoop(2048);
  spdlog::info("client running...");
  TcpClient tcpClient(eventLoop, ip, port, 4096);
  tcpClient.setMessageCallback(onMessage);

  tcpClient.send("hello world\n");
  eventLoop.loop();
  return 0;
}
