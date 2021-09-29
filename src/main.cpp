#include "tcpServer.hpp"

void onConnection(const TcpConnectionPtr &conn) {}

void onMessage(const TcpConnectionPtr &conn, char *buf, int len) {
  char sendBuf[20] = "server return";
  conn->send(sendBuf, sizeof(sendBuf));
}

int main(int argc, char const *argv[]) {
  TcpServer tcpServer(3005, "233");
  tcpServer.setConnectionCallback(onConnection);
  tcpServer.setMessageCallback(onMessage);
  return 0;
}
