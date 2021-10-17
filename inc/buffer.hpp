#ifndef __LOEVENT_BUFFER__
#define __LOEVENT_BUFFER__

// #include <cstddef>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <vector>

namespace loevent {

class Buffer {
 public:
  Buffer(size_t len) : readOffset_(0), writeOffset_(0), buffer_(len) {}
  size_t readableBytes() { return writeOffset_ - readOffset_; }
  size_t writableBytes() { return buffer_.size() - writeOffset_; }
  char* start() { return head() + readOffset_; }
  char* end() { return head() + writeOffset_; }

  void retrieve(size_t len) {
    int rb = readableBytes();
    assert(len <= rb);
    if (len < rb) {
      readOffset_ += len;
    } else {
      retrieveAll();
    }
  }
  void retrieveUntil(const char* pos) {
    assert(start() <= pos);
    assert(pos <= end());
    retrieve(pos - start());
  }
  void retrieveAll() {
    readOffset_ = 0;
    writeOffset_ = 0;
  }
  bool manualWrite(size_t len) {
    if (writableBytes() < len) {
      moveToHead();
      if (writableBytes() < len) {
        spdlog::error("buffer write out of range");
        return false;
      }
    }
    writeOffset_ += len;
    return true;
  }

  bool write(char* buf, size_t len) {
    if (writableBytes() < len) {
      moveToHead();
      if (writableBytes() < len) {
        spdlog::error("buffer write out of range");
        return false;
      }
    }
    ::memcpy(head(), buf, len);
    writeOffset_ += len;
    return true;
  }

  void printInfo() {
    printf("head: %p\n", this->head());
    printf("start: %p\n", this->start());
    printf("readOffset: %ld | writeOffset: %ld \n", readOffset_, writeOffset_);
    // printf("writeOffset: %ld\n", writeOffset_);
  }

  const char* findCRLF() {
    const char* crlf = std::search(start(), end(), kCRLF, kCRLF + 2);
    return crlf == end() ? NULL : crlf;
  }

  const char* findEOL() {
    const void* eol = memchr(start(), '\n', readableBytes());
    return static_cast<const char*>(eol);
  }

 private:
  std::vector<char> buffer_;
  size_t readOffset_;
  size_t writeOffset_;
  const char kCRLF[2] = {'\r', '\n'};
  char* head() { return &buffer_[0]; }
  void moveToHead() {
    int len = readableBytes();
    ::memcpy(head(), start(), len);
    readOffset_ = 0;
    writeOffset_ = len;
  }
};
typedef std::shared_ptr<Buffer> BufferPtr;

}  // namespace loevent
#endif  // !__LOEVENT_BUFFER__
