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
  if (argc < 6) {
    error_quit("Example: ./client <ip> <port> <thread_num> <connection_num> <blockSize>");
  }
  const char *ip = argv[1];
  int port = strtol(argv[2], NULL, 10);
  int threadNum = atoi(argv[3]);
  int connectionNum = atoi(argv[4]);
  int blockSize = atoi(argv[5]);

  EventLoop eventLoop(threadNum, 20480);
  // TcpClient tcpClient(eventLoop, ip, port, 4096);
  // tcpClient.setMessageCallback(onMessage);

  std::string message;
  for (int i = 0; i < blockSize; ++i) {
    message.push_back(static_cast<char>(i % 128));
  }

  for (int i = 0; i < connectionNum; ++i) {
    // spdlog::info("client running...");
    TcpClient *tc = new TcpClient(eventLoop, ip, port, blockSize);
    tc->setMessageCallback(onMessage);
    tc->send(message);
  }
  spdlog::info("connection ok");

  eventLoop.loop();
  return 0;
}
