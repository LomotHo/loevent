#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_client.hpp"
#include "utils.hpp"

using namespace loevent;

void onMessage(const TcpConnectionPtr &conn, char *buf, int len) {
  // spdlog::info("[onMessage] recv len: {}", len);
  // printhexDump(buf, len);
  // std::string str("hello");
  // conn->send(str);
  conn->send(buf, len);
}

int main(int argc, char const *argv[]) {
  // spdlog::set_level(spdlog::level::debug);
  if (argc < 5) {
    error_quit("Example: ./client <ip> <port> <connection_num> <blockSize>");
  }
  const char *ip = argv[1];
  int port = strtol(argv[2], NULL, 10);
  int sessionCount = atoi(argv[3]);
  int blockSize = atoi(argv[4]);

  EventLoop eventLoop(20480);
  // TcpClient tcpClient(eventLoop, ip, port, 4096);
  // tcpClient.setMessageCallback(onMessage);

  std::string message;
  for (int i = 0; i < blockSize; ++i) {
    message.push_back(static_cast<char>(i % 128));
  }

  for (int i = 0; i < sessionCount; ++i) {
    spdlog::info("client running...");
    TcpClient *tc = new TcpClient(eventLoop, ip, port, 4096);
    tc->setMessageCallback(onMessage);
    tc->send(message);
  }

  eventLoop.loop();
  return 0;
}
