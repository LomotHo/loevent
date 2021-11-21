// #include <cstddef>
#include <cstdint>

#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_server.hpp"
#include "utils.hpp"

using namespace loevent;

struct RpcData {
  std::uint64_t increaseNum;
};

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
  int RpcDataLen = sizeof(RpcData);
  if (rb >= RpcDataLen) {
    auto data = static_cast<void *>(buffer->start());
    auto data2 = static_cast<RpcData *>(data);
    data2->increaseNum++;
    conn->send(data2, RpcDataLen);
    buffer->retrieve(RpcDataLen);
  }
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
