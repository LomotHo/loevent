#ifndef __LOEVENT_HTTP_REQUEST__
#define __LOEVENT_HTTP_REQUEST__

#include <spdlog/spdlog.h>

#include <chrono>
#include <map>
#include <string>

namespace loevent {

class HttpRequest {
 public:
  enum HttpMethod { kInvalid, kGet, kPost, kHead, kPut, kDelete };
  enum HttpVersion { kUnknown, kHttp10, kHttp11 };
  void setReqLine(const char* begin, const char* end) {
    requestLine_.assign(begin, end);
    spdlog::debug("request line: {}", requestLine_);
  }
  std::string getReqLine() const { return requestLine_; }

  void setVersion(HttpVersion v) { version_ = v; }
  HttpVersion version() const { return version_; }

  bool setMethod(const char* start, const char* end) {
    assert(method_ == kInvalid);
    std::string m(start, end);
    if (m == "GET") {
      method_ = kGet;
    } else if (m == "POST") {
      method_ = kPost;
    } else if (m == "HEAD") {
      method_ = kHead;
    } else if (m == "PUT") {
      method_ = kPut;
    } else if (m == "DELETE") {
      method_ = kDelete;
    } else {
      method_ = kInvalid;
    }
    return method_ != kInvalid;
  }
  HttpMethod method() const { return method_; }
  const char* methodString() const {
    const char* result = "UNKNOWN";
    switch (method_) {
      case kGet:
        result = "GET";
        break;
      case kPost:
        result = "POST";
        break;
      case kHead:
        result = "HEAD";
        break;
      case kPut:
        result = "PUT";
        break;
      case kDelete:
        result = "DELETE";
        break;
      default:
        break;
    }
    return result;
  }

  void setPath(const char* start, const char* end) { path_.assign(start, end); }
  const std::string& path() const { return path_; }

  void setQuery(const char* start, const char* end) { query_.assign(start, end); }
  const std::string& query() const { return query_; }

  // void setReceiveTime(Timestamp t) { receiveTime_ = t; }

  // Timestamp receiveTime() const { return receiveTime_; }

  void addHeader(const char* begin, const char* colon, const char* end) {
    std::string filed(begin, colon);
    std::string value(colon + 2, end);
    spdlog::debug("header: {}:{}", filed, value);
    headers_[filed] = value;
  }
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
  HttpMethod method_ = kInvalid;
  HttpVersion version_;
  std::string path_;
  std::string query_;
  std::string receiveTime_;
  std::map<std::string, std::string> headers_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_REQUEST__
