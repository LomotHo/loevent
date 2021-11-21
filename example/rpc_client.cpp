#include "event_loop.hpp"
#include "spdlog/spdlog.h"
#include "tcp_client.hpp"
#include "utils.hpp"

using namespace loevent;
int IncreaseNum = 0;
struct RpcData {
  std::uint64_t increaseNum;
};

void onMessage(const TcpConnectionPtr &conn) {
  auto buffer = conn->getRecvBuffer();
  int rb = buffer->readableBytes();
  int RpcDataLen = sizeof(RpcData);
  if (rb >= RpcDataLen) {
    auto data = static_cast<void *>(buffer->start());
    auto data2 = static_cast<RpcData *>(data);
    IncreaseNum += 2;
    data2->increaseNum++;
    if (data2->increaseNum != IncreaseNum) {
      spdlog::error("increase err {},{}", data2->increaseNum, IncreaseNum);
    }
    // spdlog::debug("increase {},{}", data2->increaseNum, IncreaseNum);
    conn->send(data2, RpcDataLen);
    buffer->retrieve(RpcDataLen);
  }
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

  // std::string message;
  // for (int i = 0; i < blockSize; ++i) {
  //   message.push_back(static_cast<char>(i % 128));
  // }
  RpcData rd;
  rd.increaseNum = 0;
  for (int i = 0; i < sessionCount; ++i) {
    // spdlog::info("client running...");
    TcpClient *tc = new TcpClient(eventLoop, ip, port, blockSize);
    tc->setMessageCallback(onMessage);

    tc->send(&rd, sizeof(rd));
  }
  spdlog::info("connection ok");

  eventLoop.loop();
  return 0;
}
