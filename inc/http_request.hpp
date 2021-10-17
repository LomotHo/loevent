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
  std::string getReqLine() const { return requestLine_; }
  const std::string getHeader(std::string filed) {
    if (headers_.find(filed) != headers_.end()) {
      return headers_[filed];
    } else {
      return NULL;
    }
  }
  void swap(HttpRequest& that) {
    std::swap(method_, that.method_);
    std::swap(version_, that.version_);
    path_.swap(that.path_);
    query_.swap(that.query_);
    receiveTime_.swap(that.receiveTime_);
    requestLine_.swap(that.requestLine_);
    headers_.swap(that.headers_);
  }

 private:
  std::string requestLine_;
  std::string method_;
  std::string version_;
  std::string path_;
  std::string query_;
  std::string receiveTime_;
  std::map<std::string, std::string> headers_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_REQUEST__
