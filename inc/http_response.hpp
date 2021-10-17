#ifndef __LOEVENT_HTTP_RESPONSE__
#define __LOEVENT_HTTP_RESPONSE__

#include <map>
#include <string>

#include "buffer.hpp"

namespace loevent {
class HttpResponse {
 public:
  HttpResponse(bool close) : closeConnection_(close) {}
  enum HttpStatusCode {
    kUnknown,
    k200Ok = 200,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k404NotFound = 404,
  };

  void setStatusCode(HttpStatusCode s) { statusCode_ = s; }
  HttpStatusCode getStatusCode() { return statusCode_; }

  void setCloseConnection(bool on) { closeConnection_ = on; }
  bool getCloseConnection() { return closeConnection_; }
  BufferPtr genBuffer() {
    BufferPtr buffer = std::make_shared<Buffer>(1024);
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
    buffer->write(buf, strlen(buf));
    buffer->append(statusMessage_);
    buffer->append("\r\n");

    if (getCloseConnection()) {
      buffer->append("Connection: close\r\n");
    } else {
      snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
      buffer->write(buf, strlen(buf));
      buffer->append("Connection: Keep-Alive\r\n");
    }
    for (const auto& header : headers_) {
      buffer->append(header.first);
      buffer->append(": ");
      buffer->append(header.second);
      buffer->append("\r\n");
    }
    buffer->append("\r\n");
    buffer->append(body_);
    return buffer;
  }

  void setStatusMessage(const std::string& message) { statusMessage_ = message; }

  void addHeader(const char* begin, const char* colon, const char* end) {
    std::string field(begin, colon);
    std::string value(colon + 2, end);
    spdlog::debug("header: {}:{}", field, value);
    headers_[field] = value;
  }
  void addHeader(std::string field, std::string value) { headers_[field] = value; }
  const std::string getHeader(std::string field) const {
    auto it = headers_.find(field);
    if (it != headers_.end()) {
      return it->second;
    } else {
      return NULL;
    }
  }

  void setBody(const std::string& body) { body_ = body; }

 private:
  HttpStatusCode statusCode_;
  bool closeConnection_;
  std::string statusMessage_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_RESPONSE__
