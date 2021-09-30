#include "eventLoop.hpp"
#include "spdlog/spdlog.h"
#include "tcpServer.hpp"

void onConnection(const TcpConnectionPtr &conn) {
  spdlog::info("onConnection");

  char buf[] = "welcome";
  conn->send(buf, sizeof(buf));
  spdlog::info("conn fd {}", conn->s.sockfd);
}

void onMessage(const TcpConnectionPtr &conn, char *buf, int len) {
  spdlog::info("onMessage recv: {}", buf);
  // spdlog::info("onMessage");

  char sendBuf[20] = "server return";
  conn->send(sendBuf, sizeof(sendBuf));
  // spdlog::info("sendBuf {}", sendBuf);
}

int main(int argc, char const *argv[]) {
  spdlog::set_level(spdlog::level::debug);
  EventLoop eventLoop(2048);
  TcpServer tcpServer(eventLoop, 3005, "s233", 4094);
  tcpServer.setConnectionCallback(onConnection);
  tcpServer.setMessageCallback(onMessage);
  spdlog::info("running");
  eventLoop.loop();
  return 0;
}
