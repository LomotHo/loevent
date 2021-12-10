#include <stdio.h>

#include "buffer.hpp"
#include "spdlog/spdlog.h"

using namespace loevent;

int main(int argc, char const *argv[]) {
  int count = 100000;
  BufferPtr bufferPool[count];
  for (size_t i = 0; i < count; i++) {
    bufferPool[0] = std::make_shared<Buffer>(1);
    // spdlog::info("buffer i: {}", i);
  }
}
