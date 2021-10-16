#include <stdio.h>

#include "buffer.hpp"
#include "spdlog/spdlog.h"

using namespace loevent;

int main(int argc, char const *argv[]) {
  Buffer buffer(66);
  //   //   printf("peek: %p\n", buffer.peek());
  //   buffer.printInfo();
  //   char bufTmp[4] = {'n', 'm', 's', 'l'};
  //   spdlog::info("write {}", buffer.write(bufTmp, 4));
  //   //   printf("peek: %p\n", buffer.peek());
  //   buffer.printInfo();

  //   buffer.retrieve(3);
  buffer.printInfo();

  int blockSize = 16;
  char message[blockSize];
  for (int i = 0; i < blockSize; ++i) {
    message[i] = static_cast<char>(i % 128);
  }
  spdlog::info("write {}", buffer.write(message, blockSize));
  spdlog::info("write {}", buffer.write(message, blockSize));
  buffer.retrieve(blockSize);
  buffer.printInfo();

  spdlog::info("write {}", buffer.write(message, blockSize));
  spdlog::info("write {}", buffer.write(message, blockSize));
  buffer.retrieve(blockSize);
  buffer.retrieve(blockSize);
  buffer.printInfo();

  for (size_t i = 0; i < 100; i++) {
    spdlog::info("write {}", buffer.write(message, blockSize));
    buffer.retrieve(blockSize);
    buffer.printInfo();
  }
  //   spdlog::info("peek: {0:x}", b.peek());
}
