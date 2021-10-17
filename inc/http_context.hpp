#ifndef __LOEVENT_HTTP_CONTEX__
#define __LOEVENT_HTTP_CONTEX__

#include "buffer.hpp"
#include "http_request.hpp"

namespace loevent {

enum class HttpContextStatus {
  ExpectRequestLine = 1,
  ExpectHeaders,
  ExpectBody,
  GotAll,
  Broken
};
class HttpContext {
 public:
  HttpContext() {}
  HttpContextStatus parseHttpReq(BufferPtr buffer) {
    bool hasMore = true;

    while (hasMore) {
      const char *crlf = buffer->findCRLF();
      if (crlf) {
        if (status_ == HttpContextStatus::ExpectRequestLine) {
          if (!handelReqLine(buffer->start(), crlf)) {
            spdlog::error("handelReqLine error");
          };
          status_ = HttpContextStatus::ExpectHeaders;
        } else if (status_ == HttpContextStatus::ExpectHeaders) {
          const char *colon =
              std::find(static_cast<const char *>(buffer->start()), crlf, ':');
          if (colon != crlf) {
            request_.addHeader(buffer->start(), colon, crlf);
          } else {
            status_ = HttpContextStatus::GotAll;
            hasMore = false;
          }
        }

        buffer->retrieve(crlf - buffer->start() + 2);
      } else {
        hasMore = false;
      }
    }
    return status_;
  }
  void reset() {
    status_ = HttpContextStatus::ExpectRequestLine;
    HttpRequest dummy;
    request_.swap(dummy);
  }
  HttpRequest &getHttpRequest() { return request_; }

 private:
  HttpContextStatus status_ = HttpContextStatus::ExpectRequestLine;
  HttpRequest request_;

  bool handelReqLine(const char *begin, const char *end) {
    request_.setReqLine(begin, end);
    return true;
  }
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_CONTEX__
