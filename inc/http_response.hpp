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
  void setCloseConnection(bool on) { closeConnection_ = on; }
  void setStatusMessage(const std::string& message) { statusMessage_ = message; }
  void setBody(const std::string& body) { body_ = body; }

  HttpStatusCode getStatusCode() { return statusCode_; }
  bool getCloseConnection() { return closeConnection_; }
  BufferPtr genBuffer() {
    BufferPtr buffer = std::make_shared<Buffer>(1024);
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
    // buffer->append(buf, strlen(buf));
    buffer->write(buf, strlen(buf));
    buffer->append(statusMessage_);
    buffer->append("\r\n");

    if (getCloseConnection()) {
      buffer->append("Connection: close\r\n");
    }

    buffer->append("\r\n");
    buffer->append(body_);
    return buffer;
  }

 private:
  HttpStatusCode statusCode_;
  bool closeConnection_;
  std::string statusMessage_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

}  // namespace loevent
#endif  // !__LOEVENT_HTTP_RESPONSE__
