#ifndef __LOEVENT_BUFFER__
#define __LOEVENT_BUFFER__

#include <spdlog/spdlog.h>

// #include <cstddef>
// #include <cstdint>

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

  // bool manualWrite(size_t len) {
  //   if (len <= writableBytes()) {
  //     writeOffset_ += len;
  //     return true;
  //   }
  //   if (writableBytes() < len && len < MaxLength_ - writeOffset_) {
  //     int resize =
  //         buffer_.size() + writableBytes() > len ? buffer_.size() * 2 : MaxLength_;
  //     buffer_.resize(resize);
  //   }
  //   if (0) {
  //     if (MaxLength_ - readableBytes() < len) {
  //       spdlog::error("write too many once");
  //       return false;
  //     }
  //     if (MaxLength_ < buffer_.size() + len) {
  //     }
  //     buffer_.resize(buffer_.size() * 2 > MaxLength_ ? buffer_.size() * 2 :
  //     MaxLength_); if (writableBytes() < len) {
  //       spdlog::error("buffer write out of range");
  //       return false;
  //     }
  //     if (buffer_.size() == MaxLength_) {
  //       moveToBegin();
  //     }
  //   }
  // }
  bool manualWrite(size_t len) {
    if (writableBytes() < len) {
      spdlog::error("buffer error");
      return false;
    }
    if (writableBytes() == len) {
      if (buffer_.size() < MaxLength_) {
        buffer_.resize(buffer_.size() * 2 > MaxLength_ ? buffer_.size() * 2 : MaxLength_);
      } else if (buffer_.size() == MaxLength_ && writeOffset_ != 0) {
        moveToBegin();
      }
    }
    writeOffset_ += len;
    return true;
  }

  bool write(const char* buf, size_t len) {
    if (writableBytes() < len) {
      spdlog::error("buffer error");
      return false;
    }
    if (writableBytes() == len) {
      if (buffer_.size() < MaxLength_) {
        buffer_.resize(buffer_.size() * 2 > MaxLength_ ? buffer_.size() * 2 : MaxLength_);
      } else if (buffer_.size() == MaxLength_ && writeOffset_ != 0) {
        moveToBegin();
      }
    }
    ::memcpy(end(), buf, len);
    writeOffset_ += len;
    return true;
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
  // const size_t MaxLength_ = 10240;
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
