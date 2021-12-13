#ifndef __LOEVENT_BUFFER__
#define __LOEVENT_BUFFER__

#include <spdlog/spdlog.h>

#include "utils.hpp"

namespace loevent {

class Buffer {
 public:
  Buffer(size_t initLength = 1024, size_t maxLength = 10240)
      : MaxLength_(maxLength), buffer_(initLength < maxLength ? initLength : maxLength) {}
  size_t readableBytes() { return writeOffset_ - readOffset_; }
  size_t writableBytes() { return buffer_.size() - writeOffset_; }
  size_t maxWritableBytes() { return MaxLength_ - writeOffset_; }

  char* start() { return begin() + readOffset_; }
  char* end() { return begin() + writeOffset_; }

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
    if (MaxLength_ < len + readOffset_) {
      spdlog::error("write to buffer too much");
      return false;
    } else if (writableBytes() <= len) {
      if (buffer_.size() < MaxLength_) {
        int resize = writeOffset_ + len;
        resize = resize > buffer_.size() * 2 ? resize : buffer_.size() * 2;
        resize = resize < MaxLength_ ? resize : MaxLength_;
        buffer_.resize(resize);
        // spdlog::debug("[debug] buffer resize: {} | size: {}", resize, buffer_.size());
      }
      if (writableBytes() <= len && writeOffset_ != 0) {
        // spdlog::debug("[debug] buffer moveToBegin | size: {}", buffer_.size());
        moveToBegin();
      }
    }
    writeOffset_ += len;
    return true;
  }
  bool write(const char* buf, size_t len) {
    if (manualWrite(len)) {
      ::memcpy(end() - len, buf, len);
      return true;
    } else {
      return false;
    }
  }

  void append(const std::string& str) { write(str.c_str(), str.length()); }
  void append(const char* str, size_t len) { write(str, len); }

  const char* findCRLF() {
    const char* crlf = std::search(start(), end(), kCRLF, kCRLF + 2);
    return crlf == end() ? NULL : crlf;
  }

  const char* findEOL() {
    const void* eol = memchr(start(), '\n', readableBytes());
    return static_cast<const char*>(eol);
  }

  void printInfo() {
    printf("head: %p\n", this->begin());
    printf("start: %p\n", this->start());
    printf("readOffset: %ld | writeOffset: %ld \n", readOffset_, writeOffset_);
    printHexDump(this->start(), this->end() - this->start());
  }

 private:
  std::vector<char> buffer_;
  const size_t MaxLength_;
  size_t writeOffset_ = 0;
  size_t readOffset_ = 0;
  const char kCRLF[2] = {'\r', '\n'};

  char* begin() { return &*buffer_.begin(); }
  const char* begin() const { return &*buffer_.begin(); }
  void moveToBegin() {
    if (readOffset_ != 0) {
      int len = readableBytes();
      ::memcpy(begin(), start(), len);
      readOffset_ = 0;
      writeOffset_ = len;
    }
  }
};
typedef std::shared_ptr<Buffer> BufferPtr;

}  // namespace loevent
#endif  // !__LOEVENT_BUFFER__
