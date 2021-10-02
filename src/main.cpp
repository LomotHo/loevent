#include "eventLoop.hpp"
#include "spdlog/spdlog.h"
#include "tcpServer.hpp"
#include "utils.hpp"

// void onConnection(const TcpConnectionPtr &conn) {
//   spdlog::info("onConnection");

//   char buf[] = "welcome";
//   conn->send(buf, sizeof(buf));
//   spdlog::info("conn fd {}", conn->s.sockfd);
// }

void onMessage(const TcpConnectionPtr &conn, char *buf, int len) {
  spdlog::info("onMessage recv: {}", buf);
  // spdlog::info("onMessage");

  char sendBuf[] = "server-return\n";
  // conn->send(sendBuf, strlen(sendBuf));
  std::vector<char> vSendBuf(sendBuf, sendBuf + len);
  conn->send(vSendBuf);

  // conn->send(buf, len);
  // Socket s(5);
  // s.socketSend(buf, len);
  // spdlog::info("sendBuf {}", sendBuf);
}

int main(int argc, char const *argv[]) {
  spdlog::set_level(spdlog::level::debug);
  EventLoop eventLoop(2048);
  spdlog::info("running");
  TcpServer tcpServer(eventLoop, 3005, "s233", 4094);
  // tcpServer.setConnectionCallback(onConnection);
  tcpServer.setMessageCallback(onMessage);
  eventLoop.loop();
  return 0;
}
