#ifndef __LOEVENT_HTTP_REQUEST__
#define __LOEVENT_HTTP_REQUEST__

#include <spdlog/spdlog.h>

#include <map>
#include <string>

namespace loevent {

class HttpRequest {
 public:
  void setReqLine(const char* begin, const char* end) {
    requestLine_.assign(begin, end);
    spdlog::debug("request line: {}", requestLine_);
  }
  void addHeader(const char* begin, const char* colon, const char* end) {
    std::string filed(begin, colon);
    std::string value(colon + 2, end);
    spdlog::debug("header: {}:{}", filed, value);
    headers_[filed] = value;
  }

 private:
  std::string requestLine_;
  std::map<std::string, std::string> headers_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_REQUEST__
