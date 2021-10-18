#ifndef __LOEVENT_HTTP_CONTEX__
#define __LOEVENT_HTTP_CONTEX__

#include "buffer.hpp"
#include "http/http_request.hpp"
#include "io_context.hpp"

namespace loevent {

enum class HttpContextStatus {
  ExpectRequestLine = 1,
  ExpectHeaders,
  ExpectBody,
  GotAll,
  Broken
};
class HttpContext : public IoContext {
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
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space)) {
      start = space + 1;
      space = std::find(start, end, ' ');
      if (space != end) {
        const char *question = std::find(start, space, '?');
        if (question != space) {
          request_.setPath(start, question);
          request_.setQuery(question, space);
        } else {
          request_.setPath(start, space);
        }
        start = space + 1;
        succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
        if (succeed) {
          if (*(end - 1) == '1') {
            request_.setVersion(HttpRequest::kHttp11);
          } else if (*(end - 1) == '0') {
            request_.setVersion(HttpRequest::kHttp10);
          } else {
            succeed = false;
          }
        }
      }
    }
    request_.setReqLine(begin, end);
    return succeed;
  }
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_CONTEX__
